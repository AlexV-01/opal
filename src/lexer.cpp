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

static int MAX_OP_LEN = sorted_operators().front().length();
static int MIN_OP_LEN = sorted_operators().back().length();

static int MAX_SEP_LEN = sorted_separators().front().length();
static int MIN_SEP_LEN = sorted_separators().back().length();

class LexError: public std::exception {
private:
    int32_t line;
    int32_t charIdx;

public:
    LexError(int32_t l, int32_t c) : std::exception() {
        l = line;
        c = charIdx;
    }
};

std::vector<Token> lex_file(std::string fileName) {
    std::vector<Token> list;

    std::ifstream file(fileName);

    while (!file.eof()) {
        beginning:
        // CHECK FOR A NEW LINE
        if (file.peek() == '\n') {
            list.push_back(new Token(NEWLINE));
            goto beginning;
        }

        // IGNORE ANY WHITESPACE
        while (std::isspace(file.peek())) {
            file.get();
        }

        // CHECK FOR AN OPERATOR
        char buffer[MAX_OP_LEN];
        file.read(buffer, MAX_OP_LEN);
        int i = MAX_OP_LEN;
        while (i >= MIN_OP_LEN) {
            char bufstr[i];
            for (int j = 0; j < i; j++) bufstr[j] = buffer[j];
            bufstr[i] = '\0';
            std::string bufstr(bufstr);
            for (std::string op : sorted_operators()) {
                if (strcmp(op, bufstr) == 0) {
                    list.push_back(new Token(OPERATORS[op], op));
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
        char buffer[MAX_SEP_LEN];
        file.read(buffer, MAX_SEP_LEN);
        int i = MAX_SEP_LEN;
        while (i >= MIN_SEP_LEN) {
            char bufstr[i];
            for (int j = 0; j < i; j++) bufstr[j] = buffer[j];
            bufstr[i] = '\0';
            std::string bufstr(bufstr);
            for (std::string sep : sorted_separators()) {
                if (strcmp(sep, bufstr) == 0) {
                    list.push_back(new Token(SEPARATORS[sep], sep));
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
                // THROW LEX ERROR
            }
            if (is_int) {
                list.push_back(new Token(INT_LITERAL, std::stoi(acc)));
            } else if (is_int == false) {
                list.push_back(new Token(FLOAT_LITERAL, std::stof(acc)));
            }
            goto beginning;
        }

        // ANYTHING PAST THIS POINT IS AN IDENTIFIER
        if (!(std::isalpha(file.peek()) || file.peek() == '_')) {
            // THROW LEX ERROR
        } else {
            std::string acc = "";
            while (std::isalnum(file.peek()) || file.peek() == '_') {
                acc += file.get();
            }
            char* identifier = malloc(acc.length());
            strcpy(identifier, acc);
            list.push_back(new Token(IDENTIFIER, identifier));
        }
    }

    file.close();
    return list;
}