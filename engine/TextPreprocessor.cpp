#include "TextPreprocessor.h"
#include "InvertedIndex.h"
#include "trie.h"
#include "StopWords.h"
#include "BloomFilter.h"
#include "Stemmer.h"
#include <iostream>
#include <fstream>
#include <string_view>
#include <algorithm>
#include <filesystem>
#include <string>
#include <mutex>
extern std::mutex index_mutex;

using namespace std;
namespace fs = std::filesystem;

int extract_words_simplified(int currentDocID, const string& filename, BloomFilter& bloom)
{
    int words_counter = 0;

    ifstream file(filename);
    if(!file) return 0;

    string content((istreambuf_iterator<char>(file)),    
                    istreambuf_iterator<char>());        

    size_t word_start = 0;
    bool in_word = false;

    // 1. Create local structures so we dont touch the globals yet
    unordered_map<string, doc_info> local_map;
    vector<string> local_bloom_words;
    vector<string> local_trie_words;

    for(size_t i = 0; i < content.size(); ++i)
    {
        bool is_space = isspace(static_cast<unsigned char>(content[i])) || content[i] == '-';  

        if(!in_word && !is_space)
        {
            word_start = i;
            in_word = true;
        }
        else if(in_word && is_space)
        {
            string_view word_window(&content[word_start], i - word_start);
            string word(word_window);
            transform(word.begin(), word.end(), word.begin(), ::tolower);

            string clean_word = "";
            for(char c : word)
            {
                if((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) clean_word += c;
            }
            
            if(!clean_word.empty() && !global_stopwords.isStopWord(clean_word)) 
            {
                string stemmed_word = porter_stem(clean_word);
                
                // Push to our local vectors instead of the globals
                local_bloom_words.push_back(stemmed_word);

                words_counter++;
                local_trie_words.push_back(clean_word);
                
                local_map[stemmed_word].frequency++;
                local_map[stemmed_word].positions.push_back(words_counter);
            }

            in_word = false;
        }
    }
    
    if(in_word)
    {
        string_view word_window(&content[word_start], content.size() - word_start);
        string word(word_window);
        transform(word.begin(), word.end(), word.begin(), ::tolower);

        string clean_word = "";
        for(char c : word)
        {
            if((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) clean_word += c;
        }
        
        if(!clean_word.empty() && !global_stopwords.isStopWord(clean_word)) 
        {
            string stemmed_word = porter_stem(clean_word);
            
            local_bloom_words.push_back(stemmed_word);

            words_counter++;
            local_trie_words.push_back(clean_word);
            
            local_map[stemmed_word].frequency++;
            local_map[stemmed_word].positions.push_back(words_counter);
        }
    }

    // 2. We have finished reading the entire file 
    // NOW we lock the bathroom door EXACTLY ONCE to merge our local data into the globals.
    std::lock_guard<std::mutex> lock(index_mutex);

    for(const auto& pair : local_map) {
        Inverted_Index[pair.first][currentDocID] = pair.second;
    }
    for(const string& word : local_bloom_words) {
        bloom.add(word);
    }
    for(const string& word : local_trie_words) {
        global_trie.insert(word);
    }

    return words_counter;
}
