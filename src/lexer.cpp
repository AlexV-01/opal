#include "lexer.hpp"
#include <fstream>

std::vector<Token> lex_file(std::string fileName)
{
    std::vector<Token> list;

    std::ifstream file(fileName);
    while()
    {
        //read
        //add to list
        //...
    }

    char nextChar = file.get();
    char nextChar = file.peek();

    file.close();
    return list;
}