#ifndef TRIE_H
#define TRIE_H
#include<string>
#include<vector>
using namespace std;

class Node{
    public:
       Node* links[26];
        bool flag;
        Node();
        bool found_key(char ch);
        void put(char ch, Node* node);
        Node* get(char ch);
        void set_end();
        bool is_end();
};

class Trie{
    private:
        Node* root;
    public:
        Trie();
        void insert(string word);
        bool search(string word);
        Node* startswith(string prefix);
        void dfs(Node* node, vector<string>& results, string curr_word);
};

extern Trie global_trie;

#endif