#include "./reader.h"

#include "./error.h"

const Token& Reader::peek() const {
    if (tokens.empty()) {
        throw EOFError();
    }
    return *tokens.front();
}

void Reader::read() {
    if (tokens.empty()) {
        throw EOFError();
    }
    auto token = std::move(tokens.front());
    tokens.pop_front();
    if (token->getType() == TokenType::LEFT_PAREN) {
        auto next = peek();
        if (next.isDot()) {
            throw new SyntaxError(". cannot be the first token in a list");
        }
        return readTail();
    } else if (auto quoteName = token->getQuoteName()) {
        
    } else {
        
    }
}