#ifndef READER_H
#define READER_H

#include <deque>
#include <memory>

#include "./token.h"

class Reader {
private:
    std::deque<std::unique_ptr<Token>> tokens;

    const Token& peek() const;

    void readTail();

public:
    Reader() = default;
    Reader(std::deque<std::unique_ptr<Token>> tokens) : tokens{std::move(tokens)} {}

    void read();
};

#endif