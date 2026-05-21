#include "InvertedIndex.h"
#include "trie.h"
#include <iostream>
#include <fstream>
#include <string_view>
#include <algorithm>
#include <filesystem>
#include <string>

using namespace std;
namespace fs = std::filesystem;

int extract_words_simplified(int currentDocID, const string& filename)
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
    
        bool is_space = isspace(static_cast<unsigned char>(content[i]));  

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
                if(c >= 'a' && c <= 'z') clean_word += c;
            }
            if(clean_word.empty()) continue;

            words_counter++;
            if(Inverted_Index.find(clean_word) == Inverted_Index.end())
            {
                global_trie.insert(clean_word);
            }
            
            Inverted_Index[clean_word][currentDocID].frequency++;


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
            if(c >= 'a' && c <= 'z') clean_word += c;
        }

        words_counter++;
        if(Inverted_Index.find(clean_word) == Inverted_Index.end())
        {
            global_trie.insert(clean_word);
        }

        Inverted_Index[clean_word][currentDocID].frequency++;
    }
    return words_counter;
}
