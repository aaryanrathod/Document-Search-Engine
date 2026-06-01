#include "InvertedIndex.h"
#include "trie.h"
#include "TextPreprocessor.h"
#include "BloomFilter.h"
#include "LRUCache.h"
#include "QueryExpander.h"
#include "StopWords.h"
#include "Stemmer.h"
#include <iostream>
#include <algorithm>
#include <filesystem> // Required for directory iteration
#include <string>
#include <chrono> //for measuring time taken
#include <conio.h>
#include <cmath>
#include <unordered_set>

using namespace std;
namespace fs = std::filesystem;

vector<string> split_by_space(const string& str) {
    vector<string> res;
    size_t s = 0, e = str.find(' ');
    while (e != string::npos) {
        if (e != s) res.push_back(str.substr(s, e - s));
        s = e + 1;
        e = str.find(' ', s);
    }
    if (s < str.length()) res.push_back(str.substr(s));
    return res;
}

int main()
{
    cout << "Starting main..." << endl;

    string folderPath = "./data"; 
    BloomFilter global_bloom(10000);

    auto start_time_ind_gen = chrono::high_resolution_clock::now();
    
    // incremental indexing
    bool index_loaded = false;
    if (fs::exists("index.dat") && fs::exists("trie.dat")) {
        cout << "Loading Index and Trie from disk..." << endl;
        if (load_index("index.dat") && global_trie.load_trie("trie.dat")) {
            index_loaded = true;
            // Rebuild the Bloom Filter from the loaded Inverted Index keys
            for (const auto& pair : Inverted_Index) {
                global_bloom.add(pair.first);
            }
        }
    }

    // Build a set of existing file paths so we know which ones to skip
    unordered_set<string> seen_files;
    int max_id = 0;
    if (index_loaded) {
        for (const auto& doc : corpus) {
            seen_files.insert(doc.filepath);
            if (doc.id > max_id) max_id = doc.id;
        }
    }
    
    int doc_id = max_id + 1; // Start ID for new documents
    int new_files_added = 0;

    try {
        if (fs::exists(folderPath) && fs::is_directory(folderPath)) {
            for (const auto& entry : fs::directory_iterator(folderPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                    if (entry.path().filename().string() == "synonyms.txt") continue;
                    
                    string fileName = entry.path().filename().string();
                    string fullPath = entry.path().string();            

                    // Incremental Check: if file not seen before, update it
                    if (seen_files.find(fullPath) == seen_files.end()) {
                        cout << "Indexing new file: " << fileName << endl;
                        add_doc(doc_id, fileName, fullPath, global_bloom);
                        doc_id++; 
                        new_files_added++;
                    }
                }
            }
        } else {
            cerr << "Error: Directory path does not exist." << endl;
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Filesystem Error: " << e.what() << endl;
    }

    // If new files were added (or we built from scratch), save to disk for next time!
    if (new_files_added > 0 || !index_loaded) {
        cout << "Saving new index to disk..." << endl;
        save_index("index.dat");
        global_trie.save_trie("trie.dat");
    }


    //Finding average document length
    double avg_doc_len = 0.0;
    for(int i = 0; i < corpus.size(); i++)
    {
        avg_doc_len += corpus[i].total_words;
    }
    avg_doc_len = avg_doc_len / corpus.size();

    // Pre-calculate IDF scores for all words in the index
    double N = corpus.size();
    for (const auto& pair : Inverted_Index) {
        const string& word = pair.first;
        double n = pair.second.size();
        IDF_Table[word] = log(((N - n + 0.5) / (n + 0.5)) + 1.0);
    }
    
    auto end_time_ind_gen = chrono::high_resolution_clock::now();
    auto duration_ind_gen = chrono::duration_cast<chrono::milliseconds>(end_time_ind_gen - start_time_ind_gen);

    cout << "Engine Boot Time: " << duration_ind_gen.count() << " milliseconds" << endl;

    LRUCache global_cache(10);
    QueryExpander global_expander("./data/synonyms.txt");

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

                if(node != NULL)
                {
                    //exact prefix found
                    global_trie.dfs(node, results, curr_word);
                }
                else if(curr_word.length() >= 4)
                {
                     // Exact match failed. now ask fuzzy search
                    vector<string> fuzzy_results = global_trie.fuzzy_autocomplete(curr_word, 1);
                    for(const string& f : fuzzy_results) 
                    {
                        results.push_back(f + " (Did you mean?)");
                    }
                }

                //Inject synonyms/acronyms into suggestions
                vector<string> ui_expansions = global_expander.getExpansions({curr_word});
                for(const string& exp : ui_expansions) {
                    results.push_back(exp + " (Query expanded suggestion)");
                }
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
        
        // Clean the user's query: Allow numbers, collapse duplicate spaces, treat hyphens as spaces
        string clean_query = "";
        bool last_was_space = true; // Trims leading spaces
        for(char c : query)
        {
            if(c == '-') c = ' ';
            
            if((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
                clean_query += c;
                last_was_space = false;
            }
            else if(c == ' ' && !last_was_space) {
                clean_query += ' ';
                last_was_space = true;
            }
        }
        // Trim trailing space
        if(!clean_query.empty() && clean_query.back() == ' ') {
            clean_query.pop_back();
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
        if(start < query.length())
        {
            tokens.push_back(query.substr(start));
        }

        vector<string> filtered_tokens;
        for(string& it : tokens)
        {
            if(!global_stopwords.isStopWord(it)){
                // If the stemmed word isnt in the index, it might be a typo...
                if(!global_bloom.mightContain(porter_stem(it)) && it.length() >= 4) {
                    vector<string> fuzzy_res = global_trie.fuzzy_autocomplete(it, 1); // Allow 1 typo
                    if(!fuzzy_res.empty()) {
                        cout << "\n[Auto-corrected '" << it << "' to '" << fuzzy_res[0] << "']" << endl;
                        it = fuzzy_res[0]; // Fix the typo before we stem
                    }
                }
                filtered_tokens.push_back(porter_stem(it));
            }
        }
        tokens = filtered_tokens;

        
        if(tokens.empty()) {
            cout << "No non-stopwords found in query.\n\n";
            continue;
        }


        vector<string> raw_expanded_tokens = global_expander.getExpansions(tokens);
        vector<string> expanded_tokens;
        for (const string& exp : raw_expanded_tokens) {
            vector<string> sub = split_by_space(exp);
            for(const string& s : sub) {
                if(!global_stopwords.isStopWord(s)) {
                    expanded_tokens.push_back(porter_stem(s));
                }
            }
        }

        auto start_time_bloom = chrono::high_resolution_clock::now();
        // Bloom Filter Bouncer
        bool bloom_blocked = false;
        for(const string& token : tokens) {
            bool token_exists = global_bloom.mightContain(token);
            
            // If the original token doesn't exist, check if ANY of its expansions exist
            if(!token_exists) {
                vector<string> single_expansion_raw = global_expander.getExpansions({token});
                vector<string> single_expansion;
                for (const string& exp : single_expansion_raw) {
                    vector<string> sub = split_by_space(exp);
                    for(const string& s : sub) {
                        if(!global_stopwords.isStopWord(s)) {
                            single_expansion.push_back(porter_stem(s));
                        }
                    }
                }
                for(const string& exp : single_expansion) {
                    if(global_bloom.mightContain(exp)) {
                        token_exists = true;
                        break;
                    }
                }
            }

            if(!token_exists) {
                bloom_blocked = true;
                break; // If ANY token (and all its expansions) are missing, block
            }
        }
        
        auto end_time_bloom = chrono::high_resolution_clock::now();
        auto duration_bloom = chrono::duration_cast<chrono::microseconds>(end_time_bloom - start_time_bloom);
        if(bloom_blocked) {
            cout << "\n" << duration_bloom.count() << "microseconds" << endl;
            cout << "No documents found for '" << query << "' (Blocked instantly by Bloom Filter)\n\n";
            continue; // Skip BM25 entirely
        }

        // Score Accumulation
        unordered_map<int, double> document_scores;
        
        // 1. Score Original Tokens (1.0x Weight)
        for(int i = 0; i < tokens.size(); i++)
        {
            if(Inverted_Index.find(tokens[i]) != Inverted_Index.end())
            {
                // Lookup pre-calculated IDF
                double idf = IDF_Table[tokens[i]];

                //Loop through every document that contains the word
                for(auto pair : Inverted_Index[tokens[i]])
                {
                    int docID = pair.first;
                    int f = pair.second.frequency;
                    double D = corpus[docID - 1].total_words;

                    // calculate tf_bm25 component
                    double tf_bm25 = (f * (1.2 + 1.0)) / (f + 1.2 * (1.0 - 0.75 + 0.75 * (D / avg_doc_len)));
                
                    // Final Score (1.0x Multiplier for Exact Matches)
                    document_scores[docID] += (idf * tf_bm25) * 1.0;
                }
            }
        }

        // 2. Score Expanded Tokens (0.2x Weight)
        for(int i = 0; i < expanded_tokens.size(); i++)
        {
            if(Inverted_Index.find(expanded_tokens[i]) != Inverted_Index.end())
            {
                // Lookup pre-calculated IDF
                double idf = IDF_Table[expanded_tokens[i]];

                for(auto pair : Inverted_Index[expanded_tokens[i]])
                {
                    int docID = pair.first;
                    int f = pair.second.frequency;
                    double D = corpus[docID - 1].total_words;

                    double tf_bm25 = (f * (1.2 + 1.0)) / (f + 1.2 * (1.0 - 0.75 + 0.75 * (D / avg_doc_len)));
                
                    // Final Score (0.2x Multiplier for Synonyms/Acronyms)
                    document_scores[docID] += (idf * tf_bm25) * 0.2;
                }
            }
        }

        // 3. Phrasal Boosting
        // If the user typed "ml" (size 1), but it expanded to "machine", "learning" (size 2), 
        // we should boost documents that contain the exact phrase "machine learning"
        const vector<string>& sequence_tokens = (tokens.size() == 1 && expanded_tokens.size() > 1) ? expanded_tokens : tokens;

        if(sequence_tokens.size() > 1)
        {
            for(auto& pair : document_scores)
            {
                int docID = pair.first;

                //check if all the sequence tokens are present in the document
                bool all_exist = true;
                for(const string& token : sequence_tokens)
                {
                    if(Inverted_Index[token].find(docID) == Inverted_Index[token].end())
                    {
                        all_exist = false;
                        break;
                    }
                }

                if(all_exist)
                {
                    vector<int>& first_positions = Inverted_Index[sequence_tokens[0]][docID].positions;
                    bool sequence_found = false;

                    for(int pos : first_positions)
                    {
                        bool match = true;
                        for(int i = 1; i < sequence_tokens.size(); i++)
                        {
                            vector<int>& next_positions = Inverted_Index[sequence_tokens[i]][docID].positions;
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
        
        // global_cache.put will happen after sorting
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
        
        if(!bm25_rank.empty())
        {
            sort(bm25_rank.rbegin(), bm25_rank.rend()); // Highest to Lowest
            global_cache.put(query, bm25_rank); // CACHE IT AFTER SORTING
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