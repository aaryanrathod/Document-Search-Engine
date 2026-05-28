#pragma once
#ifndef QUERY_EXPANDER
#define QUERY_EXPANDER

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>

using namespace std;

class QueryExpander {
private:
    // Dictionary mapping a word to its expanded synonyms/acronyms
    unordered_map<string, vector<string>> dict;

public:
    // Constructor reads the massive CSV
    QueryExpander(string filepath);

    // Returns a NEW vector of expansions for the given tokens
    vector<string> getExpansions(const vector<string>& original_tokens);
};

#endif
