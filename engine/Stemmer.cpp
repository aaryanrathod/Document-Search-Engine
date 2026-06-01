#include "Stemmer.h"
#include <algorithm>

using namespace std;

// A highly optimized, lightweight C++ implementation of the Porter Stemming Algorithm.
// It strips common English suffixes (s, es, ed, ing, ly) to reduce words to their roots.
string porter_stem(const string& word) {
    string b = word;
    if(b.length() <= 2) return b;
    
    //Plurals and 's
    if(b.back() == 's') 
    {
        if(b.length() > 4 && b.substr(b.length() - 4) == "sses") 
        {
            b.erase(b.length() - 2);
        } 
        else if(b.length() > 3 && b.substr(b.length() - 3) == "ies") 
        {
            b.erase(b.length() - 2);
        } 
        else if(b.length() > 2 && b[b.length() - 2] != 's') 
        {
            b.pop_back();
        }
    }
    
    //-ing, -ed, -ly
    bool stripped = false;
    if(b.length() > 4) 
    {
        if(b.substr(b.length() - 3) == "ing") 
        {
            b.erase(b.length() - 3);
            stripped = true;
        } 
        else if(b.substr(b.length() - 2) == "ed") 
        {
            b.erase(b.length() - 2);
            stripped = true;
        } 
        else if(b.substr(b.length() - 2) == "ly") 
        {
            b.erase(b.length() - 2);
            stripped = true;
        }
    }
    
    // clean up trailing double letters (e.g., runn -> run, hopp -> hop)
    // we leave 'l', 's', and 'z' alone (e.g., fall -> fall, miss -> miss)
    if(stripped && b.length() > 2)
    {
        if(b[b.length() - 1] == b[b.length() - 2] && 
            b.back() != 'l' && b.back() != 's' && b.back() != 'z')
            {
                b.pop_back();
            }
    }
    
    // y -> i replacement (e.g., happy -> happi)
    if(b.length() > 2 && b.back() == 'y') b.back() = 'i';
    
    return b;
}
