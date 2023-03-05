#include "./reader.h"

#include "./error.h"

void Reader::checkEmpty() {
    while (tokens.empty()) {
        if (!eofHandler || !eofHandler(topLevel)) {
            throw EOFError();
        }
    }
}

const Token* Reader::peek() {
    checkEmpty();
    return tokens.front().get();
}

TokenPtr Reader::pop() {
    checkEmpty();
    auto top = std::move(tokens.front());
    tokens.pop_front();
    return top;
}

std::unique_ptr<Value> Reader::readValue() {
    auto token = pop();
    topLevel = false;
    if (token->getType() == TokenType::LEFT_PAREN) {
        auto next = peek();
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
    if (peek()->getType() == TokenType::RIGHT_PAREN) {
        tokens.pop_front();
        return std::make_unique<NilValue>();
    }
    auto car = readValue();
    std::unique_ptr<Value> cdr;
    if (peek()->getType() == TokenType::DOT) {
        tokens.pop_front();
        cdr = readValue();
        if (tokens.empty()) {
            throw SyntaxError("Unexpected EOF; expect an element after .");
        }
        auto token = pop();
        if (token->getType() != TokenType::RIGHT_PAREN) {
            throw SyntaxError("Expected exactly one element after .");
        }
    } else {
        cdr = readTails();
    }
    return std::make_unique<PairValue>(std::move(car), std::move(cdr));
}

std::unique_ptr<Value> Reader::read() {
    topLevel = true;
    return readValue();
}