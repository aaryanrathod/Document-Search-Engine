#include "InvertedIndex.h"
#include "trie.h"
#include "TextPreprocessor.h"
#include "LRUCache.h"
#include <iostream>
#include <algorithm>
#include <filesystem> // Required for directory iteration
#include <string>
#include <chrono> //for measuring time taken
#include <conio.h>
#include <cmath>

using namespace std;
namespace fs = std::filesystem;

int main()
{

    string folderPath = "./data"; 
    int doc_id = 1; // Start counter for document IDs

    auto start_time_ind_gen = chrono::high_resolution_clock::now();
    try {
        // 2. Verify the directory exists

        if (fs::exists(folderPath) && fs::is_directory(folderPath)) {
            
            // 3. Loop through all files in the directory
            for (const auto& entry : fs::directory_iterator(folderPath)) {
                
                // 4. Check if the file is a regular file and has a .txt extension
                if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                    
                    // Extract strings needed for your add_doc function
                    string fileName = entry.path().filename().string(); // e.g., "doc1.txt"
                    string fullPath = entry.path().string();            // Full absolute path

                    // 5. Pass the variables into your function
                    add_doc(doc_id, fileName, fullPath);
                    
                    // 6. Increment ID for the next file
                    doc_id++; 

                }
            }
        } else {
            cerr << "Error: Directory path does not exist or is incorrect." << endl;
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Filesystem Error: " << e.what() << endl;
    }

    //Finding average document length
    double avg_doc_len = 0.0;
    for(int i = 0; i < corpus.size(); i++)
    {
        avg_doc_len += corpus[i].total_words;
    }
    avg_doc_len = avg_doc_len / corpus.size();

    auto end_time_ind_gen = chrono::high_resolution_clock::now();
    auto duration_ind_gen = chrono::duration_cast<chrono::milliseconds>(end_time_ind_gen - start_time_ind_gen);

    cout << "Engine Boot Time: " << duration_ind_gen.count() << " milliseconds" << endl;

    LRUCache global_cache(10);

    while(true)
    {
        string query = "";
        cout << "Enter a word to search (or type '/exit' to quit): ";

        
        while(true)
        {
            
            int ch = _getch();

            // 13 is Enter key
            if(ch == 13) break;
            
            //8 is backspace
            if(ch == 8)
            {
                if(!query.empty())
                {
                    query.pop_back();
                    //move cursoor back, overwrite with space, move back again
                    cout << "\b \b" << flush;
                }
            }

            else if(ch >= 32 && ch <= 126)
            {
                char lower_ch = tolower(static_cast<unsigned char>(ch));
                query += lower_ch;
                cout << lower_ch << flush;
            }

            auto start_time_typahd = chrono::high_resolution_clock::now();
            //trie search
            vector<string> results;
            string past_words = "";
            
            if(!query.empty())
            {
                //1. Split the string at the last space  (e.g: machine learning)
                string curr_word = query;
                
                size_t last_space = query.find_last_of(' '); //find the index of the last space
                if(last_space != string::npos)
                {
                    past_words = query.substr(0, last_space + 1);  //"machine "
                    curr_word = query.substr(last_space + 1);      //"lea"
                }

                //2. Search in the trie with only the current word

                Node* node = nullptr;
                if(!curr_word.empty())
                {
                    node = global_trie.startswith(curr_word);
                }

                if(node != NULL) global_trie.dfs(node, results, curr_word);
            }
            
            auto end_time_typahd = chrono::high_resolution_clock::now();
            
            auto duration_typahd = chrono::duration_cast<chrono::microseconds>(end_time_typahd - start_time_typahd);
           
            system("cls"); //clear terminal screen

            cout << "\nSuggestions: \n";

            for(int j = 0; j < results.size(); j++)
            {
                cout << " -> " << past_words << results[j] << endl;
            }
            
            cout << endl << "Search: " << query;
            
            cout << "\n\n[Typeahead latency: " << duration_typahd.count() << "microseconds]"<<endl;
        }


        if(query == "/exit") break; 
        
        transform(query.begin(), query.end(), query.begin(), ::tolower);
        
        // Strip punctuation from the user's query just like TextPreprocessor does for documents
        string clean_query = "";
        for(char c : query)
        {
            if(c >= 'a' && c <= 'z') clean_query += c;
            else if(c == ' ') clean_query += ' '; // We must preserve spaces to split tokens later!
        }
        query = clean_query;

        auto start_time_cached = chrono::high_resolution_clock::now();

        const vector<pair<double, int>>& cached_results = global_cache.get(query);

        vector<pair<double, int>> bm25_rank;

        auto end_time_cached = chrono::high_resolution_clock::now();
        auto duration_cached = chrono::duration_cast<chrono::microseconds>(end_time_cached - start_time_cached);

        if(!cached_results.empty())
        {
            cout << "Results for " << query << " (Searched in " << duration_cached.count() << " microseconds):" << endl;
            cout<<"CACHE HIT"<<endl;
            cout << "\nYour words can be found in these documents:\n";
            for(int i = 0; i < cached_results.size(); i++)
            {
                cout << "{File: " << corpus[cached_results[i].second - 1].filepath << ", " << "BM25 Score: " << cached_results[i].first << "}" << endl;
            }
            cout << endl;
            continue;
        }
        auto start_time = chrono::high_resolution_clock::now();

        vector<string> tokens;
        
        size_t start = 0;
        size_t end = query.find(' ');

        while(end != string::npos)
        {
            if(end != start)    //prevent empty tokens if there are double spaces
            {
                tokens.push_back(query.substr(start, end - start));
            }
            start = end + 1;
            end = query.find(' ', start);    //the new end will be the space after the start
        }
        //push the final word
        if(start < query.length())
        {
            tokens.push_back(query.substr(start));
        }

        //Score Accumulation
        unordered_map<int, double> document_scores;
        for(int i = 0; i < tokens.size(); i++)
        {
            if(Inverted_Index.find(tokens[i]) != Inverted_Index.end())
            {
                //calculating IDF
                double N = corpus.size();
                double n = Inverted_Index[tokens[i]].size();
                double idf  = log( ((N - n + 0.5) / (n + 0.5)) + 1.0 );

                //Loop through every document that contains the word
                for(auto pair : Inverted_Index[tokens[i]])
                {
                    int docID = pair.first;
                    int f = pair.second.frequency;

                    // Get the length of this specific document
                    // (Assuming your docIDs start at 1, the index in corpus is docID - 1)
                    double D = corpus[docID - 1].total_words;

                    // calculate tf_bm25 component
                    double tf_bm25 = (f * (1.2 + 1.0)) / (f + 1.2 * (1.0 - 0.75 + 0.75 * (D / avg_doc_len)));
                
                    // 4. Final Score
                    document_scores[docID] += idf * tf_bm25;

                }
            }
        }

        if(tokens.size() > 1)
        {
            for(auto& pair : document_scores)
            {
                int docID = pair.first;

                //check if all the tokens are present in the document (AND requirement)

                bool all_exist = true;
                for(const string& token : tokens)
                {
                    if(Inverted_Index[token].find(docID) == Inverted_Index[token].end())
                    {
                        all_exist = false;
                        break;
                    }
                }

                if(all_exist)
                {
                    vector<int>& first_positions = Inverted_Index[tokens[0]][docID].positions;
                    bool sequence_found = false;

                    for(int pos : first_positions)
                    {
                        bool match = true;
                        //checking if word 2 is at pos + 1, word 3 is at pos + 2, etc.
                        for(int i = 1; i < tokens.size(); i++)
                        {
                            vector<int>& next_positions = Inverted_Index[tokens[i]][docID].positions;
                            //we do a binary search as the text preprocessor already puts positions in ascending order
                            if(!binary_search(next_positions.begin(), next_positions.end(), pos + i))
                            {
                                match = false;
                                break;
                            }
                        }
                        if(match)
                        {
                            sequence_found = true;
                            break;
                        }
                    }

                    if(sequence_found)
                    {
                        document_scores[docID] += 100.0; //we will give a massive phrasal boost
                    }
                }
            }
        }

        
        //Sort and print
        for(auto pair : document_scores)
        {
            // Flip it to {score, docID} for sorting
            bm25_rank.push_back({pair.second, pair.first});
        }
        
        global_cache.put(query, bm25_rank);

        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
        
        if(!bm25_rank.empty())
        {
            sort(bm25_rank.rbegin(), bm25_rank.rend()); // Highest to Lowest
            cout << "Results for '" << query << "' (Searched in " << duration.count() << " microseconds):" << endl;
            cout << "\nYour words can be found in these documents:\n";
            for(int i = 0; i < bm25_rank.size(); i++)
            {
                cout << "{File: " << corpus[bm25_rank[i].second - 1].filepath << ", " << "BM25 Score: " << bm25_rank[i].first << "}" << endl;
            }
            cout << endl;
        }

        else 
        {
            cout << "No documents found containing '" << query << "' (Searched in " << duration.count() << " microseconds)\n\n";
        }
        
    }
    cout << "\nThank you" <<endl;
    return 0;
}