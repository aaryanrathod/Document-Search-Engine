#include "InvertedIndex.h"
#include "trie.h"
#include "BloomFilter.h"
#include <iostream>
#include <fstream>
#include <string_view>
#include <algorithm>
#include <filesystem>
#include <string>

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
            if(clean_word.empty()) continue;

            bloom.add(clean_word); // Add the word to the Bloom Filter

            words_counter++;
            if(Inverted_Index.find(clean_word) == Inverted_Index.end())
            {
                global_trie.insert(clean_word);
            }
            
            Inverted_Index[clean_word][currentDocID].frequency++;
            Inverted_Index[clean_word][currentDocID].positions.push_back(words_counter);


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

        bloom.add(clean_word); // Add the word to the Bloom Filter

        words_counter++;
        if(Inverted_Index.find(clean_word) == Inverted_Index.end())
        {
            global_trie.insert(clean_word);
        }

        Inverted_Index[clean_word][currentDocID].frequency++;
        Inverted_Index[clean_word][currentDocID].positions.push_back(words_counter);

    }
    return words_counter;
}
