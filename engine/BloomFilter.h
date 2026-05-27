#pragma once
#ifndef BLOOM_FILTER
#define BLOOM_FILTER
#include <iostream>
#include <vector>
#include <functional>
#include <string>
using namespace std;

class BloomFilter{
    private:
        vector<bool> bit_array;
        int size;
    
    public:
        BloomFilter(int size);
        int hash1(string word);
        int hash2(string word);
        int hash3(string word);

        void add(string word);

        bool mightContain(string word);
};

#endif