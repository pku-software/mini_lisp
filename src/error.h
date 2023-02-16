#ifndef ERROR_H
#define ERROR_H

#include <exception>
#include <stdexcept>

class SyntaxError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class EOFError : public std::runtime_error {
public:
    EOFError() : runtime_error("Unexpected end of file") {}
};

class LispError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

#endif