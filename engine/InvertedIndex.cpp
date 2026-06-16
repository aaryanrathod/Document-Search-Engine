#include "InvertedIndex.h"
#include "TextPreprocessor.h"
#include "trie.h"
#include <iostream>
#include <fstream>
#include <mutex>
extern std::mutex index_mutex;

vector<Document> corpus;
unordered_map<string, unordered_map<int, doc_info>> Inverted_Index;
unordered_map<string, double> IDF_Table;

void add_doc(int id, string title, string filepath, BloomFilter& bloom)
{
    Document doc;
    doc.id = id;
    doc.title = title;
    doc.filepath = filepath;
    
    doc.total_words = extract_words_simplified(id, filepath, bloom);
    // for(auto pair : Inverted_Index)
    // {
    //     cout << pair.first<< " -> [ ";
    //     for(auto pair2 : pair.second)
    //     {
    //         cout<< "{Doc: "<< pair2.first <<", Freq: "<< pair2.second.frequency<<"} ";
    //     }
    //     cout << "]"<<endl;
    // }
    lock_guard<mutex> lock(index_mutex);
    corpus.push_back(doc);


    // cout <<"Added document: "<< title << endl;
}

void save_index(const string& filename)
{
    ofstream out(filename, ios::binary);
    if (!out) return;

    // 1. Save Corpus (Document list)
    int corpus_size = corpus.size();
    out.write((char*)&corpus_size, sizeof(int));
    for (const auto& doc : corpus) {
        out.write((char*)&doc.id, sizeof(int));
        out.write((char*)&doc.total_words, sizeof(int));
        
        int len = doc.filepath.size();
        out.write((char*)&len, sizeof(int));
        out.write(doc.filepath.c_str(), len);
    }

    // 2. Save Inverted Index
    int num_words = Inverted_Index.size();
    out.write((char*)&num_words, sizeof(int));
    for (const auto& pair : Inverted_Index) {
        int len = pair.first.size();
        out.write((char*)&len, sizeof(int));
        out.write(pair.first.c_str(), len);

        int num_docs = pair.second.size();
        out.write((char*)&num_docs, sizeof(int));

        for (const auto& doc_pair : pair.second) {
            int docID = doc_pair.first;
            int freq = doc_pair.second.frequency;
            out.write((char*)&docID, sizeof(int));
            out.write((char*)&freq, sizeof(int));

            int num_pos = doc_pair.second.positions.size();
            out.write((char*)&num_pos, sizeof(int));
            for (int pos : doc_pair.second.positions) {
                out.write((char*)&pos, sizeof(int));
            }
        }
    }
}

bool load_index(const string& filename) {
    ifstream in(filename, ios::binary);
    if (!in) return false;

    corpus.clear();
    Inverted_Index.clear();

    // 1. Load Corpus
    int corpus_size;
    if (!in.read((char*)&corpus_size, sizeof(int))) return false;
    
    for (int i = 0; i < corpus_size; ++i) {
        Document doc;
        in.read((char*)&doc.id, sizeof(int));
        in.read((char*)&doc.total_words, sizeof(int));
        
        int len;
        in.read((char*)&len, sizeof(int));
        doc.filepath.resize(len);
        in.read(&doc.filepath[0], len); // Read string directly into memory
        
        corpus.push_back(doc);
    }

    // 2. Load Inverted Index
    int num_words;
    if (!in.read((char*)&num_words, sizeof(int))) return false;

    for (int i = 0; i < num_words; ++i) {
        int len;
        in.read((char*)&len, sizeof(int));
        string word;
        word.resize(len);
        in.read(&word[0], len);

        int num_docs;
        in.read((char*)&num_docs, sizeof(int));


        for (int j = 0; j < num_docs; ++j) {
            int docID, freq, num_pos;
            in.read((char*)&docID, sizeof(int));
            in.read((char*)&freq, sizeof(int));
            in.read((char*)&num_pos, sizeof(int));

            Inverted_Index[word][docID].frequency = freq;
            Inverted_Index[word][docID].positions.resize(num_pos);
            for (int k = 0; k < num_pos; ++k) {
                in.read((char*)&Inverted_Index[word][docID].positions[k], sizeof(int));
            }
        }
    }
    return true;
}
