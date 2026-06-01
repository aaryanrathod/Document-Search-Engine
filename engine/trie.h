#ifndef TRIE_H
#define TRIE_H
#include<string>
#include<vector>
#include<fstream>
using namespace std;

class Node{
    public:
       Node* links[36];
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
        void fuzzy_search_recursive(Node* node, const string& query, vector<int>& prev_row, vector<string>& results, string curr_word, int max_edits);
        vector<string> fuzzy_autocomplete(string query, int max_edits = 1);
        void save_recursive(Node* node, string curr_word, std::ofstream& out);
        void save_trie(const string& filename);
        bool load_trie(const string& filename);
};

extern Trie global_trie;

#endif