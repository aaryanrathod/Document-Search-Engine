#include "QueryExpander.h"
#include <iostream>
#include <unordered_set>

using namespace std;

QueryExpander::QueryExpander(string filepath)
{
    ifstream file(filepath);
    if (!file.is_open())
    {
        cerr << "Warning: Could not open " << filepath << " for Query Expansion!" << endl;
        return;
    }

    string line;
    int loaded_count = 0;

    // Read the file line by line
    while(getline(file, line))
    {
        // Skip empty lines or comments (# headers)
        if(line.empty() || line[0] == '#' || line[0] == ' ') continue;

        stringstream ss(line);
        string word;
        vector<string> row_words;

        // Split the line by commas
        while(getline(ss, word, ','))
        {
            // Trim any trailing/leading spaces or carriage returns (e.g Windows \r)
            if(!word.empty() && word.back() == '\r') word.pop_back();
            if(!word.empty()) row_words.push_back(word);
        }

        if(row_words.size() < 2) continue; // Needs at least 2 words to be a synonym

        // Make it perfectly symmetric
        // For example: ml -> machine, learning AND machine -> ml, learning
        for(int i = 0; i < row_words.size(); i++) {
            for (int j = 0; j < row_words.size(); j++) {
                if (i != j) {
                    dict[row_words[i]].push_back(row_words[j]);
                }
            }
        }
        loaded_count++;
    }
    
    file.close();
    cout << "Query Expander successfully loaded " << loaded_count << " synonym groups from " << filepath << endl;
}


vector<string> QueryExpander::getExpansions(const vector<string>& original_tokens)
{
    unordered_set<string> unique_expansions;
    
    for (const string& token : original_tokens) {
        // If the token exists in our dictionary, add all its synonyms/acronyms
        if (dict.find(token) != dict.end()) {
            for (const string& synonym : dict[token]) {
                unique_expansions.insert(synonym);
            }
        }
    }
    
    // Convert back to a vector for the rest of the engine
    vector<string> expanded(unique_expansions.begin(), unique_expansions.end());
    return expanded;
}
