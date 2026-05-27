#include "BloomFilter.h"
using namespace std;
BloomFilter::BloomFilter(int size) : size(size), bit_array(size, false){}

int BloomFilter::hash1(string word)
{
    size_t hash_val = hash<string>{}(word);
    return hash_val % size;
}

int BloomFilter::hash2(string word)
{
    size_t hash_val = hash<string>{}(word + "_SALT_A");
    return (hash_val * 31) % size;
}

int BloomFilter::hash3(string word)
{
    size_t hash_val = hash<string>{}(word + "_SALT_B");
    return (hash_val * 37) % size;
}

void BloomFilter::add(string word)
{
    int pos1 = hash1(word);
    int pos2 = hash2(word);
    int pos3 = hash3(word);
    bit_array[pos1] = true;
    bit_array[pos2] = true;
    bit_array[pos3] = true;
}

bool BloomFilter::mightContain(string word)
{
    int pos1 = hash1(word);
    int pos2 = hash2(word);
    int pos3 = hash3(word);
    if(bit_array[pos1] && bit_array[pos2] && bit_array[pos3]) return true;
    return false;
}