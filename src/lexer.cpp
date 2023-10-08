#include "lexer.hpp"
#include <fstream>
#include <algorithm>
#include <string.h>

static std::vector<std::string> sorted_operators() {
    std::vector<std::string> ops;
    for (auto op : OPERATORS) {
        ops.push_back(op.first);
    }
    std::sort(ops.begin(), ops.end());
    reverse(ops.begin(), ops.end());
    return ops;
}

static std::vector<std::string> sorted_separators() {
    std::vector<std::string> seps;
    for (auto sep : SEPARATORS) {
        seps.push_back(sep.first);
    }
    std::sort(seps.begin(), seps.end());
    std::reverse(seps.begin(), seps.end());
    return seps;
}

void free_tokens(std::vector<Token> tokens) {
    for (Token t : tokens) {
        if (t.type == Token::IDENTIFIER) {
            delete t.iden;
        }
    }
}

static int MAX_OP_LEN = sorted_operators().front().length();
static int MIN_OP_LEN = sorted_operators().back().length();

static int MAX_SEP_LEN = sorted_separators().front().length();
static int MIN_SEP_LEN = sorted_separators().back().length();

class LexError: public std::exception {
private:
    int32_t line;
    int32_t charIdx;
protected:
    std::string get_position_str() {
        return "line " + std::to_string(line) + ":" + std::to_string(charIdx) + " - ";
    }
public:
    LexError() : std::exception() {}
    LexError(int32_t l, int32_t c) : std::exception() {
        l = line;
        c = charIdx;
    }
    std::string what() {
        return get_position_str() + "syntax error";
    }
};

std::vector<Token> lex_file(std::string fileName) {
    std::vector<Token> list;

    std::ifstream file(fileName);

    beginning:
    while (!file.eof()) {
        // CHECK FOR A NEW LINE
        if (file.peek() == '\n') {
            list.push_back(Token(Token::Type::NEWLINE));
            goto beginning;
        }

        // IGNORE ANY WHITESPACE
        while (std::isspace(file.peek())) {
            file.get();
        }

        // COMMENTS
        if(file.peek() == '?') {
            while(true) {
                // case: end
                if (file.peek() == '\n') {
                    list.push_back(Token(Token::Type::NEWLINE));
                    goto beginning;
                }
                if(file.eof()) goto beginning;

                // case:?
                file.get();
            }
        }

        // CHECK FOR AN OPERATOR
        char buffer[MAX_OP_LEN];
        file.read(buffer, MAX_OP_LEN);
        int i = MAX_OP_LEN;
        while (i >= MIN_OP_LEN) {
            char bufstr[i];
            for (int j = 0; j < i; j++) bufstr[j] = buffer[j];
            bufstr[i] = '\0';
            std::string buffrange(bufstr);
            for (std::string op : sorted_operators()) {
                if (op == buffrange) {
                    list.push_back(Token(Token::OPERATOR, OPERATORS.at(op)));
                    while (i > 0) {
                        file.get();
                        i--;
                    }
                    goto beginning;
                }
            }
            i--;
        }

        // CHECK FOR A SEPARATOR
        char buffer2[MAX_SEP_LEN];
        file.read(buffer2, MAX_SEP_LEN);
        i = MAX_SEP_LEN;
        while (i >= MIN_SEP_LEN) {
            char bufstr[i];
            for (int j = 0; j < i; j++) bufstr[j] = buffer2[j];
            bufstr[i] = '\0';
            std::string buffrange(bufstr);
            for (std::string sep : sorted_separators()) {
                if (sep == buffrange) {
                    list.push_back(Token(Token::SEPARATOR, SEPARATORS.at(sep)));
                    while (i > 0) {
                        file.get();
                        i--;
                    }
                    goto beginning;
                }
            }
            i--;
        }

        // CHECK FOR AN INTEGER OR FLOAT LITERAL
        bool is_number = std::isdigit(file.peek()) || file.peek() == '.' ? true : false;
        bool is_int = true;
        if (is_number) {
            std::string acc = "";
            while (is_int && !file.eof()) {
                if (file.peek() == '.') {
                    is_int = false;
                    acc += file.get();
                    break;
                }
                if (std::isdigit(file.peek())) {
                    acc += file.get();
                } else {
                    break;
                }
            }
            while (!is_int && !file.eof()) { // only executes for float literals
                if (!std::isdigit(file.peek())) break;
                acc += file.get();
            }
            if (acc == ".") {
                throw new LexError();
            }
            if (is_int) {
                list.push_back(Token(Token::Type::INT_LITERAL, std::stoi(acc)));
            } else if (is_int == false) {
                list.push_back(Token(Token::Type::FLOAT_LITERAL, std::stof(acc)));
            }
            goto beginning;
        }

        // ANYTHING PAST THIS POINT IS AN IDENTIFIER
        if (!(std::isalpha(file.peek()) || file.peek() == '_')) {
            throw new LexError();
        } else {
            std::string acc = "";
            char* acc_arr = new char[acc.length() + 1];
            strcpy(acc_arr, acc.c_str());
            while (std::isalnum(file.peek()) || file.peek() == '_') {
                acc += file.get();
            }
            list.push_back(Token(Token::IDENTIFIER, acc_arr));
        }
    }

    file.close();
    return list;
}