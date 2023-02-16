#ifndef READER_H
#define READER_H

#include <deque>
#include <memory>

#include "./token.h"
#include "./value.h"

class Reader {
private:
    std::deque<std::unique_ptr<Token>> tokens;

    bool empty() const {
        return tokens.empty();
    }
    const Token* peek() const;
    std::unique_ptr<Value> readTails();

public:
    Reader() = default;
    Reader(std::deque<std::unique_ptr<Token>> tokens) : tokens{std::move(tokens)} {}

    std::unique_ptr<Value> read();
};

#endif