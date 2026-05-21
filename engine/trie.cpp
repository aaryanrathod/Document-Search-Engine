#include "trie.h"

using namespace std;

Node::Node() {
    flag = false;
    for (int i = 0; i < 26; i++) {
        links[i] = nullptr;
    }
}
bool Node::found_key(char ch)
{
    if(ch < 'a' || ch > 'z') return false;
    return (links[ch - 'a'] != NULL);
}

void Node::put(char ch, Node* node)
{
    links[ch - 'a'] = node;
}

Node* Node::get(char ch)
{
    if(ch < 'a' || ch > 'z') return NULL;
    return links[ch - 'a'];
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
    return node->is_end(); // Simplified your original if-else block
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
    if(results.size() >= 5) return;
    if(node->is_end()) results.push_back(curr_word);
    for(int i = 0; i < 26; i++)
    {
        if(node->links[i] != NULL)
        {
            dfs(node->links[i], results, curr_word + char(i + 'a'));
        }
    }
}


 

Trie global_trie;
