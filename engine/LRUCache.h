#pragma once
#include<iostream>
#include<list>
#include<unordered_map>
#include<vector>
#include<string>

using namespace std;

class LRUCache{
    private:
        int capacity;
        list<string> lru_list;
        unordered_map<string, pair<vector<pair<double, int>>, list<string>::iterator>> cache_map;
    
    public:
        LRUCache(int cap);
        const vector<pair<double, int>>& get(string query);
        void put(string query, vector<pair<double, int>> results);
};