#include "LRUCache.h"

LRUCache::LRUCache(int cap) 
{
    capacity = cap;
}

const vector<pair<double, int>>& LRUCache::get(string query) 
{
    auto it = cache_map.find(query); // Hash once
    if(it == cache_map.end())
    {
        static const vector<pair<double, int>> empty_vec = {};
        return empty_vec;
    }

    lru_list.splice(lru_list.begin(), lru_list, it->second.second); 
    return it->second.first;
}

void LRUCache::put(string query, vector<pair<double, int>> results) 
{
    // If the query already exists, update its results and move it to front
    if(cache_map.find(query) != cache_map.end()) {
        cache_map[query].first = results;
        lru_list.splice(lru_list.begin(), lru_list, cache_map[query].second);
        return;
    }
    // If the cache is full, pop the oldest query from the back of the list and erase from the map
    if(lru_list.size() == capacity) {
        string last_key = lru_list.back();
        lru_list.pop_back();
        cache_map.erase(last_key);
    }
    // Insert the new query at the front of the list and map it to its results and its new list iterator
    lru_list.push_front(query);
    cache_map[query] = {results, lru_list.begin()};
}
