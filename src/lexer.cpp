#include "lexer.hpp"
#include <fstream>
#include <algorithm>
#include <string.h>

static std::vector<std::string> sorted_operators() {
    std::vector<std::string> ops;
    for (auto op : OPERATORS) {
        ops.push_back(op.first);
    }
    std::sort(ops.begin(), ops.end()).reverse();
    return ops;
}

static std::vector<std::string> sorted_separators() {
    std::vector<std::string> seps;
    for (auto sep : SEPARATORS) {
        seps.push_back(sep.first);
    }
    std::sort(seps.begin(), seps.end()).reverse();
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
}

std::vector<Token> lex_file(std::string fileName) {
    std::vector<Token> list;

    std::ifstream file(fileName);

    while (!file.eof()) {
        beginning:
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
        bool is_int = true;
        //std::isdigit('5');
        std::string acc;
        while (is_int) {
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
        while (!is_int) { // only executes for float literals

        }
    }

    file.close();
    return list;
}