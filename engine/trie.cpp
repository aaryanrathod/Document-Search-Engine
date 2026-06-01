#include "trie.h"
#include <algorithm>
using namespace std;

Node::Node() {
    flag = false;
    for (int i = 0; i < 36; i++) {
        links[i] = nullptr;
    }
}
bool Node::found_key(char ch)
{
    if(ch >= 'a' && ch <= 'z') return (links[ch - 'a'] != NULL);
    if(ch >= '0' && ch <= '9') return (links[26 + (ch - '0')] != NULL);
    return false;
}

void Node::put(char ch, Node* node)
{
    if(ch >= 'a' && ch <= 'z') links[ch - 'a'] = node;
    else if(ch >= '0' && ch <= '9') links[26 + (ch - '0')] = node;
}

Node* Node::get(char ch)
{
    if(ch >= 'a' && ch <= 'z') return links[ch - 'a'];
    if(ch >= '0' && ch <= '9') return links[26 + (ch - '0')];
    return NULL;
}

void Node::set_end(){
    flag = true;
}

bool Node::is_end()
{
    return flag;
}

Trie::Trie(){
    root  = new Node();
}

void Trie::insert(string word)
{
    Node* node = root;
            for(int i = 0; i < word.size(); i++)
            {
                if(!node->found_key(word[i]))
                {
                    node->put(word[i], new Node());
                }
                node = node->get(word[i]);
            }
            node->set_end();
}

bool Trie::search(string word) {
    Node* node = root;
    for (size_t i = 0; i < word.size(); i++) {
        if (!node->found_key(word[i])) return false;
        node = node->get(word[i]);
    }
    return node->is_end();
}

Node* Trie::startswith(string prefix)
{
    Node* node = root;
    for(int i  = 0; i < prefix.size(); i++)
    {
        if(!node->found_key(prefix[i])) return NULL;
        node = node->get(prefix[i]);
    }
    return node;
}

void Trie::dfs(Node* node, vector<string>& results, string curr_word)
{
    if(results.size() >= 5) return; //stop after finding 5 words

    if(node->is_end())
    {
        results.push_back(curr_word);
    }

    for(int i = 0; i < 36; i++)
    {
        if(node->links[i] != NULL)
        {
            char next_ch;
            if(i < 26) next_ch = i + 'a';
            else next_ch = (i - 26) + '0';
            dfs(node->links[i], results, curr_word + next_ch);
        }
    }
}

void Trie::fuzzy_search_recursive(Node* node, const string& query, vector<int>& prev_row, vector<string>& results, string curr_word, int max_edits) {
    if (results.size() >= 5) return; // Stop if we have enough suggestions

    int cols = query.size() + 1;
    vector<int> curr_row(cols);
    curr_row[0] = prev_row[0] + 1;

    int min_dist = curr_row[0];
    char ch = curr_word.back();

    // Calculate the Levenshtein row for the current character
    for (int i = 1; i < cols; ++i) 
    {

        int insert_cost = curr_row[i - 1] + 1;
        int delete_cost = prev_row[i] + 1;
        int replace_cost = prev_row[i - 1] + (query[i - 1] == ch ? 0 : 1);
        curr_row[i] = min({insert_cost, delete_cost, replace_cost});
        min_dist = min(min_dist, curr_row[i]);
    }

    // Dont search in the branch if it has too many errors
    if (min_dist > max_edits) return; 

    // If the edit distance from the current word to the full query is okay okay
    if (curr_row.back() <= max_edits)
    {
        // Switch to exact autocomplete from this node downward
        dfs(node, results, curr_word);
    } else 
    {
        // Otherwise, continue exploring fuzzy paths
        for (int i = 0; i < 36; ++i) 
        {
            if (node->links[i] != NULL) 
            {
                char next_ch = (i < 26) ? (i + 'a') : ((i - 26) + '0');
                fuzzy_search_recursive(node->links[i], query, curr_row, results, curr_word + next_ch, max_edits);
            }
        }
    }
}

vector<string> Trie::fuzzy_autocomplete(string query, int max_edits) 
{
    vector<string> results;
    // Dont apply typo tolerance to tiny words (e.g., 'a', 'on', 'is')
    if (query.length() < 4) return results; 
    
    int cols = query.size() + 1;
    vector<int> first_row(cols);
    for (int i = 0; i < cols; ++i) first_row[i] = i;

    // Boot up the recursive search for every root branch
    for (int i = 0; i < 36; ++i) 
    {
        if (root->links[i] != NULL) 
        {
            char next_ch = (i < 26) ? (i + 'a') : ((i - 26) + '0');
            string curr_word = "";
            curr_word += next_ch;
            fuzzy_search_recursive(root->links[i], query, first_row, results, curr_word, max_edits);
        }
    }
    return results;
}

void Trie::save_recursive(Node* node, string curr_word, std::ofstream& out) {
    // If we hit the end of a valid word, write it to disk!
    if (node->is_end()) {
        int len = curr_word.length();
        out.write((char*)&len, sizeof(int));
        out.write(curr_word.c_str(), len);
    }
    // Continue exploring the Trie
    for (int i = 0; i < 36; ++i) {
        if (node->links[i] != NULL) {
            char next_ch = (i < 26) ? (i + 'a') : ((i - 26) + '0');
            save_recursive(node->links[i], curr_word + next_ch, out);
        }
    }
}

void Trie::save_trie(const string& filename) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) return;
    save_recursive(root, "", out);
}

bool Trie::load_trie(const string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) return false;
    
    while (true) {
        int len;
        // Try to read the next word length. If it fails, we reached the end of the file!
        if (!in.read((char*)&len, sizeof(int))) break; 
        
        string word;
        word.resize(len);
        in.read(&word[0], len);
        
        // Instantly rebuild the Trie by inserting the word
        insert(word); 
    }
    return true;
}


 

Trie global_trie;
