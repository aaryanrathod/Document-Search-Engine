#include "InvertedIndex.h"
#include "trie.h"
#include "TextPreprocessor.h"
#include <iostream>
#include <algorithm>
#include <filesystem> // Required for directory iteration
#include <string>
#include <chrono> //for measuring time taken
#include <conio.h>

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
            
            if(!query.empty())
            {
                Node* node = global_trie.startswith(query);

                if(node != NULL) global_trie.dfs(node, results, query);
            }
            
            auto end_time_typahd = chrono::high_resolution_clock::now();
            
            auto duration_typahd = chrono::duration_cast<chrono::microseconds>(end_time_typahd - start_time_typahd);
           
            system("cls"); //clear terminal screen

            cout << "\nSuggestions: \n";

            for(int j = 0; j < results.size(); j++)
            {
                cout << results[j] << "    " << endl;
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

        if(found)
        {
            cout << "Results for '" << query << "' (Found in " << duration.count() << " microseconds):" << endl;
            
            for(auto pair : Inverted_Index[query])
            {
                cout << " -> {Document " << pair.first << " with frequency: " << pair.second.frequency << "}\n";
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