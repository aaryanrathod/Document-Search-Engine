# C++ In-Memory Search Engine

A custom-built, high-performance local search engine written in C++17. This project implements a fully functional inverted index, sub-millisecond typeahead autocomplete, and the Okapi BM25 ranking algorithm to search through a corpus of text documents.

## Core Features

* BM25 Ranking: Calculates Term Frequency and Inverse Document Frequency to rank search results mathematically rather than relying on basic keyword counts.
* Trie-Based Typeahead: Implements a custom Trie data structure to provide instant autocomplete suggestions as the user types, with latencies frequently under 100 microseconds.
* Phrasal Boosting: If search terms appear consecutively in a document, the engine uses an optimized binary search algorithm on the positional vectors to identify the sequence and apply a massive score boost.
* Zero-Allocation Tokenization: Avoids the heavy heap allocation overhead of stringstreams by utilizing manual pointers and std::string_view to parse documents.

## Project Structure

* engine/main.cpp: The core loop, query processor, and BM25 math.
* engine/InvertedIndex.cpp / .h: Defines the core inverted index hash map and document structures.
* engine/TextPreprocessor.cpp: Parses incoming .txt files, extracts raw text, filters punctuation, and maps word positions.
* engine/trie.cpp: The autocomplete prefix tree implementation.
* data/: The directory where the corpus of text documents is stored.

## Building and Running

You will need a C++17 compatible compiler like GCC. 

1. Clone the repository to your local machine.
2. Ensure you have your target .txt files inside the data folder.
3. Open your terminal and compile the engine using the following command:

g++ -std=c++17 engine/main.cpp engine/InvertedIndex.cpp engine/TextPreprocessor.cpp engine/trie.cpp -o SearchEngine.exe

4. Run the executable:

.\SearchEngine.exe

5. Type your query into the console. The engine will suggest words as you type. Press Enter to execute the full BM25 search. Type /exit to close the engine.
