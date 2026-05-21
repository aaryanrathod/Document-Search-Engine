#include "InvertedIndex.h"
#include "TextPreprocessor.h"
#include "trie.h"
#include<iostream>

vector<Document> corpus;
unordered_map<string, unordered_map<int, doc_info>> Inverted_Index;

void add_doc(int id, string title, string filepath)
{
    Document doc;
    doc.id = id;
    doc.title = title;
    doc.filepath = filepath;
    
    doc.total_words = extract_words_simplified(id, filepath);
    // for(auto pair : Inverted_Index)
    // {
    //     cout << pair.first<< " -> [ ";
    //     for(auto pair2 : pair.second)
    //     {
    //         cout<< "{Doc: "<< pair2.first <<", Freq: "<< pair2.second.frequency<<"} ";
    //     }
    //     cout << "]"<<endl;
    // }
    corpus.push_back(doc);


    cout <<"Added document: "<< title << endl;
}