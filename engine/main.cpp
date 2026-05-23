#include "InvertedIndex.h"
#include "trie.h"
#include "TextPreprocessor.h"
#include <iostream>
#include <algorithm>
#include <filesystem> // Required for directory iteration
#include <string>
#include <chrono> //for measuring time taken
#include <conio.h>
#include <cmath>

using namespace std;
namespace fs = std::filesystem; // Alias for readability

int main()
{
    // 1. Define the absolute path to your folder
    string folderPath = "C:/Users/ASUS/OneDrive/Documents/Mini-Search/data"; 
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

        auto start_time = chrono::high_resolution_clock::now();

        bool found = Inverted_Index.find(query) != Inverted_Index.end();

        auto end_time = chrono::high_resolution_clock::now();

        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

        vector<pair<double, int>> bm25_rank;
        if(found)
        {
            cout << "Results for '" << query << "':" << endl;
            
            // 1. Calculate Inverse Document Frequency
            double N = corpus.size();
            double n = Inverted_Index[query].size();
            double idf = log( ((N - n + 0.5) / (n + 0.5)) + 1.0 );
            
            // 2. Loop through every document that contains the word
            for(auto pair : Inverted_Index[query])
            {
                int docID = pair.first;
                int f = pair.second.frequency;
                
                // Get the length of this specific document
                // (Assuming your docIDs start at 1, the index in corpus is docID - 1)
                double D = corpus[docID - 1].total_words; 
                
                // 3. Calculate the BM25 TF component
                double tf_bm25 = (f * (1.2 + 1.0)) / (f + 1.2 * (1.0 - 0.75 + 0.75 * (D / avg_doc_len)));
                
                // 4. Final Score
                double bm25_score = idf * tf_bm25;
                bm25_rank.push_back({bm25_score, docID});
                
                //cout << " -> {Document " << docID << " | BM25 Score: " << bm25_score << "}\n";
            }
            sort(bm25_rank.rbegin(), bm25_rank.rend()); //Highest to Lowest
            cout << "\nYour words can be found in these documents:\n";
            for(int i = 0; i < bm25_rank.size(); i++)
            {
                cout << "{DocID: " << bm25_rank[i].second << ", " << "bm25_rank: " << bm25_rank[i].first << "}" <<endl;
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