# C++ In-Memory Search Engine

A custom-built, high-performance local search engine written in C++17. This project implements a fully functional inverted index, sub-millisecond typeahead autocomplete, and the Okapi BM25 ranking algorithm to search through a corpus of text documents. It features advanced probabilistic data structures and aggressive memory optimizations to achieve extremely low latency.

## Core Features

* **BM25 Ranking:** Calculates Term Frequency and Inverse Document Frequency to rank search results mathematically rather than relying on basic keyword counts.
* **Trie-Based Typeahead:** Implements a custom Trie data structure to provide instant autocomplete suggestions as the user types, with latencies frequently under 100 microseconds.
* **Phrasal Boosting:** If search terms appear consecutively in a document, the engine uses an optimized binary search algorithm on the positional vectors to identify the exact sequence and apply a massive score boost.
* **Zero-Copy LRU Cache:** A Strict-Capacity Least Recently Used (LRU) Cache powered by a `std::list` and `std::unordered_map`. It caches heavily searched queries and utilizes C++ references (`&`) to achieve true zero-copy retrieval, dropping search latency to 1-4 microseconds.
* **Bloom Filter Bouncer:** A custom, probabilistic Bloom Filter that acts as a query "bouncer". It uses double-hashing (prime-number salt seeding) to instantly intercept and block searches for missing words in `O(1)` time, completely bypassing the BM25 mathematical overhead for true negative queries.
* **Zero-Allocation Tokenization:** Avoids the heavy heap allocation overhead of stringstreams by utilizing manual pointers and `std::string_view` to parse documents.

## Project Structure

* `engine/main.cpp`: The core loop, query processor, dependency injection setup, and BM25 math.
* `engine/InvertedIndex.cpp / .h`: Defines the core inverted index hash map and document structures.
* `engine/TextPreprocessor.cpp`: Parses incoming `.txt` files, extracts raw text, filters punctuation, and populates the data structures.
* `engine/trie.cpp`: The autocomplete prefix tree implementation.
* `engine/LRUCache.cpp`: The O(1) doubly-linked list caching architecture.
* `engine/BloomFilter.cpp`: The probabilistic bit-array rejection architecture.
* `data/`: The directory where the corpus of text documents is stored.

## Building and Running

You will need a C++17 compatible compiler like GCC. 

1. Clone the repository to your local machine.
2. Ensure you have your target `.txt` files inside the `data` folder.
3. Open your terminal and compile the engine using the following command:

```bash
g++ -std=c++17 engine/main.cpp engine/InvertedIndex.cpp engine/TextPreprocessor.cpp engine/trie.cpp engine/LRUCache.cpp engine/BloomFilter.cpp -o SearchEngine.exe
```

4. Run the executable:

```bash
.\SearchEngine.exe
```

5. Type your query into the console. The engine will suggest words as you type. Press Enter to execute the full BM25 search. Type `/exit` to close the engine.
