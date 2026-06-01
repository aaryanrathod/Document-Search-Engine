#  High-Performance C++ Search Engine

A blazingly fast, zero-dependency, full-text search engine built entirely from scratch in C++17. 

This project goes far beyond a simple string-matcher. It is a complete Information Retrieval architecture inspired by enterprise systems like Elasticsearch and Lucene, featuring **BM25 scoring**, **Levenshtein Typo Tolerance**, **Binary Incremental Indexing**, and a **Probabilistic Bloom Filter**.

##  Core Architecture & Features

###  Enterprise-Grade Systems
* **Typo Tolerance (Fuzzy Search):** A dynamic programming Levenshtein algorithm woven directly into a custom Trie structure. It detects spelling errors in real-time and autocorrects them in `O(prefix)` time.
* **Incremental Binary Serialization:** The engine serializes its massive Inverted Index and Corpus directly from RAM to pure binary bytes on the hard drive (`index.dat`). This allows the engine to boot in `< 1 millisecond`. If new documents are added, the engine performs incremental indexing—appending only the new files without needing a full rebuild.
* **Probabilistic Guarding (Bloom Filter):** Implements a highly optimized, bit-level Bloom Filter to act as an `O(1)` bouncer. If a user searches for a word that doesn't exist in the corpus, the Bloom Filter instantly blocks the query, saving expensive disk/index lookups.
* **Algorithmic Porter Stemmer & Stopwords:** A custom NLP pipeline that algorithmically strips English suffixes (e.g., `-ing`, `-ed`, `-ly`, plurals) to reduce words to their roots, dramatically increasing search recall without sacrificing precision.

###  Search & Retrieval Mechanics
* **BM25 Scoring Algorithm:** Implements the industry-standard `Okapi BM25` mathematical formula, featuring dynamic TF-IDF and average document length normalization to rank the most relevant documents first.
* **Query Expansion (Synonyms):** A highly modular dictionary system that detects acronyms/synonyms (e.g., `ml` -> `machine learning`) and seamlessly expands the query tokens before they hit the index.
* **Phrasal Boosting:** Positional arrays are stored inside the Inverted Index, allowing the engine to calculate exact word adjacency. Searching `"artificial intelligence"` dynamically boosts the score of documents where the words appear perfectly side-by-side.
* **Live Typeahead Autocomplete:** A custom N-ary Trie structure captures keystrokes via `getch()` (POSIX/Windows compliant) to provide instant, sub-millisecond search suggestions as the user types.
* **LRU Cache:** An `O(1)` Least-Recently-Used caching mechanism built with a doubly-linked list and unordered map. Identical queries are intercepted and returned instantly without re-calculating BM25 scores.

##  Technical Implementation
* **Language:** C++17
* **Dependencies:** None (Zero external libraries. Fully written from scratch).
* **Cross-Platform:** Uses Preprocessor Macros (`#ifdef _WIN32`) to ensure flawless compilation across Windows (MSVC), Linux (GCC), and macOS (Clang) using standard POSIX headers.
* **Memory Management:** Highly optimized pass-by-reference structures, custom destructors, and raw binary streams to minimize heap allocations and maximize cache locality.

##  Getting Started

1. **Compile:** `g++ -std=c++17 engine/*.cpp -o SearchEngine`
2. **Run:** `./SearchEngine`
3. **Data:** Drop any `.txt` files into the `./data/` folder. The engine will automatically detect them, index them, and update the binary `index.dat` file.
