#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <deque>

#include "./token.h"

class Tokenizer {
private:
    std::unique_ptr<Token> nextToken(int& pos);

    std::string input;
public:
    Tokenizer() = default;
    Tokenizer(const std::string& input): input{input} {}

    std::deque<std::unique_ptr<Token>> tokenize();
};

#endif