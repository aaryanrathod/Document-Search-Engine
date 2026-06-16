#ifndef PARSER_H
#define PARSER_H

#include <string>

#include "BloomFilter.h"


int extract_words_simplified(int currentDocID, const std::string& filename, BloomFilter& bloom);

#endif