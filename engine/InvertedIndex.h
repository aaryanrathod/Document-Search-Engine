#pragma once        //stops the file from included twice
#include "BloomFilter.h"

#include<string>
#include<vector>
#include<unordered_map>
using namespace std;

struct Document{
    int id;
    string title;
    string filepath;
    int total_words = 0;
};

struct doc_info{
    int frequency = 0;
    vector<int> positions;
};

extern vector<Document> corpus;
extern unordered_map<string, unordered_map<int, doc_info>> Inverted_Index;
extern unordered_map<string, double> IDF_Table;

void add_doc(int id, string title, string filepath, BloomFilter& bloom);