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

static char* read_n_bytes(std::ifstream& file, int n) {
    char* bytes = (char*)malloc(n);
    for (int i = 0; i < n; i++) {
        if (file.peek() == EOF) return bytes;
        bytes[i] = file.get();
    }
    return bytes;
}

static void free_read_bytes(char* bytes) {
    free(bytes);
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
    std::string get_position_str() const {
        return "line " + std::to_string(line) + ":" + std::to_string(charIdx) + " - ";
    }
public:
    LexError() : std::exception() {}
    LexError(int32_t l, int32_t c) : std::exception() {
        l = line;
        c = charIdx;
    }
    const char* what() const noexcept override {
        return (get_position_str() + "syntax error").c_str();
    }
};

std::vector<Token> lex_file(std::string fileName) {
    std::vector<Token> list;

    std::ifstream file(fileName);
    if (!file.good())
        return std::vector<Token>();

    beginning:
    while (file.peek() != EOF) {
        // REMOVE ALL WHITESPACE BEFORE POTENTIAL NEW LINE
        while (std::isspace(file.peek()) && file.peek() != '\n') {
            file.get();
        }
        
        // CHECK FOR A NEW LINE
        if (file.peek() == '\n') {
            file.get();
            if (list.size() > 0 && list.back().type != Token::Type::NEWLINE) {
                list.push_back(Token(Token::Type::NEWLINE));
            }
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
                    file.get();
                    if (list.size() > 0 && list.back().type != Token::Type::NEWLINE) {
                        list.push_back(Token(Token::Type::NEWLINE));
                    }
                    goto beginning;
                }
                if(file.eof()) goto beginning;

                // case:?
                file.get();
            }
        }

        // CHECK FOR AN OPERATOR
        char test = file.peek();
        size_t oldpos = file.tellg();
        char* buffer = read_n_bytes(file, MAX_OP_LEN);
        int i = MAX_OP_LEN;
        while (i >= MIN_OP_LEN) {
            char* bufstr = (char*)malloc(i+1);
            for (int j = 0; j < i; j++) bufstr[j] = buffer[j];
            bufstr[i] = '\0';
            std::string buffrange(bufstr);
            for (std::string op : sorted_operators()) {
                if (op == buffrange) {
                    list.push_back(Token(Token::OPERATOR, OPERATORS.at(op)));
                    file.seekg(oldpos + op.length(), std::ios_base::beg);
                    goto beginning;
                }
            }
            file.seekg(oldpos, std::ios_base::beg);
            free(bufstr);
            i--;
        }
        free_read_bytes(buffer);

        // CHECK FOR A SEPARATOR
        oldpos = file.tellg();
        char* buffer2 = read_n_bytes(file, MAX_SEP_LEN);
        i = MAX_SEP_LEN;
        while (i >= MIN_SEP_LEN) {
            char* bufstr = (char*)malloc(i+1);
            for (int j = 0; j < i; j++) bufstr[j] = buffer2[j];
            bufstr[i] = '\0';
            std::string buffrange(bufstr);

            for (std::string sep : sorted_separators()) {
                if (sep == buffrange) {
                    list.push_back(Token(Token::SEPARATOR, SEPARATORS.at(sep)));
                    file.seekg(oldpos + sep.length(), std::ios_base::beg);
                    goto beginning;
                }
            }
            file.seekg(oldpos, std::ios_base::beg);
            free(bufstr);
            i--;
        }
        free_read_bytes(buffer2);

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
                    char test = file.peek();
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
            while (std::isalnum(file.peek()) || file.peek() == '_') {
                acc += file.get();
            }
            char* acc_arr = new char[acc.length() + 1];
            strcpy(acc_arr, acc.c_str());
            list.push_back(Token(Token::IDENTIFIER, acc_arr));
        }
    }

    file.close();
    return list;
}