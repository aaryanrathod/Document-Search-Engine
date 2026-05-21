#ifndef PARSER_H
#define PARSER_H

#include <string>

// Declares the function so other files (like your main function) can call it.
// We use std::string instead of plain string because namespace std is omitted in headers.
int extract_words_simplified(int currentDocID, const std::string& filename);

#endif // PARSER_