#include "./reader.h"

#include "./error.h"

const Token* Reader::peek() const {
    if (empty()) {
        throw EOFError();
    }
    return tokens.front().get();
}

std::unique_ptr<Value> Reader::read() {
    if (empty()) {
        throw EOFError();
    }
    auto token = std::move(tokens.front());
    tokens.pop_front();
    if (token->getType() == TokenType::LEFT_PAREN) {
        auto next = peek();
        if (next->isDot()) {
            throw SyntaxError(". cannot be the first token in a list");
        }
        return readTails();
    } else if (auto quoteName = token->getQuoteName()) {
        return std::make_unique<PairValue>(
            std::make_unique<IdentifierValue>(*quoteName),
            std::make_unique<PairValue>(read(), std::make_unique<NilValue>()));
    } else if (token->getType() == TokenType::NUMERIC_LITERAL) {
        auto value = static_cast<NumericLiteralToken&>(*token).getValue();
        return std::make_unique<NumberValue>(value);
    } else if (token->getType() == TokenType::BOOLEAN_LITERAL) {
        auto value = static_cast<BooleanLiteralToken&>(*token).getValue();
        return std::make_unique<BooleanValue>(value);
    } else if (token->getType() == TokenType::STRING_LITERAL) {
        auto value = static_cast<StringLiteralToken&>(*token).getValue();
        return std::make_unique<StringValue>(value);
    } else if (token->getType() == TokenType::IDENTIFIER) {
        auto name = static_cast<IdentifierToken&>(*token).getName();
        return std::make_unique<IdentifierValue>(name);
    } else {
        throw SyntaxError("Unexpected token " + token->toString());
    }
}

std::unique_ptr<Value> Reader::readTails() {
    if (empty()) {
        throw SyntaxError("Unexpected EOF");
    }
    auto next = peek();
    if (next->getType() == TokenType::RIGHT_PAREN) {
        tokens.pop_front();
        return std::make_unique<NilValue>();
    } else if (next->isDot()) {
        tokens.pop_front();
        auto value = read();
        if (empty()) {
            throw SyntaxError("Unexpected EOF");
        }
        auto token = std::move(tokens.front());
        tokens.pop_front();
        if (token->getType() != TokenType::RIGHT_PAREN) {
            throw SyntaxError("Expected exactly one element after .");
        }
        return value;
    } else {
        auto car = read();
        auto cdr = readTails();
        return std::make_unique<PairValue>(std::move(car), std::move(cdr));
    }
}