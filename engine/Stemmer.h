#pragma once
#ifndef STEMMER_H
#define STEMMER_H

#include <string>

// External open-source C++ implementation of the Porter Stemming Algorithm
// Reduces English words to their root form (e.g., "running" -> "run")
std::string porter_stem(const std::string& word);

#endif
