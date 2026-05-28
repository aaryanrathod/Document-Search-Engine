#include "trie.h"

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


 

Trie global_trie;
