#pragma once
#ifndef STOPWORDS_H
#define STOPWORDS_H

#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <sstream>

using namespace std;

class StopWordsManager {
    private:
        unordered_set<string> stopwords;
    
    public:
        StopWordsManager(string filepath);

        bool isStopWord(const string& word);
};

extern StopWordsManager global_stopwords;

#endif