#ifndef READER_H
#define READER_H

#include <deque>
#include <memory>

#include "./token.h"
#include "./value.h"

using EofHandler = bool(bool);

class Reader {
private:
    std::deque<std::unique_ptr<Token>>& tokens;
    std::function<EofHandler> eofHandler;
    bool topLevel{true};

    void checkEmpty();
    const Token* peek();
    std::unique_ptr<Token> pop();
    std::unique_ptr<Value> readValue();
    std::unique_ptr<Value> readTails();

public:
    Reader(std::deque<std::unique_ptr<Token>>& tokenSrc, std::function<EofHandler> eofHandler = {})
        : tokens{tokenSrc}, eofHandler{std::move(eofHandler)} {}

    std::unique_ptr<Value> read();
};

#endif