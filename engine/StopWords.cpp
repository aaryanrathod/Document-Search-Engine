#include<iostream>
#include "StopWords.h"
using namespace std;

StopWordsManager::StopWordsManager(string filepath)
{
    ifstream file(filepath);
    if(!file.is_open())
    {
        cerr << "Warning: could not open " << filepath << " for stopwords extraction" << endl;
        return;
    }

    string line;
    
    while(getline(file, line))
    {

        if(line.empty() || line[0] == '#' || line[0] == ' ') continue;

        stringstream ss(line);
        string word;
        
        while(getline(ss, word))
        {
            if(!word.empty() && word.back() == '\r') word.pop_back();
            if(!word.empty()) stopwords.insert(word);
        }
    }

    file.close();
    cout << "Stopwords loaded" << endl;
}


bool StopWordsManager::isStopWord(const string& word)
{
    if(stopwords.find(word) != stopwords.end()) return true;
    return false;
}

StopWordsManager global_stopwords("./data/stopwords.txt");