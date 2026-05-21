#include <iostream>
#include <conio.h>  // For _getch()
#include <string>
using namespace std;

int main()
{
    string query = "";
    cout << "Enter the word you want to search for: " << endl;
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
            query += static_cast<char>(ch);
            cout << static_cast<char>(ch) << flush;
        }

        //trie;
    }
    cout << endl;

    cout << "Final Search Query: " << query << endl;
    return 0;
}