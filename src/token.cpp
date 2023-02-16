#include "./token.h"

#include <sstream>
#include <iomanip>

using namespace std::literals;

std::unique_ptr<Token> Token::fromChar(char c) {
    TokenType type;
    switch (c) {
        case '(': type = TokenType::LEFT_PAREN; break;
        case ')': type = TokenType::RIGHT_PAREN; break;
        case '\'':type = TokenType::QUOTE; break;
        case '`': type = TokenType::QUASIQUOTE; break;
        case ',': type = TokenType::UNQUOTE; break;
        default: return nullptr;
    }
    return std::make_unique<Token>(Token(type));
}

bool Token::isDot() const {
    return type == TokenType::IDENTIFIER && static_cast<const IdentifierToken&>(*this).getName() == ".";
}

std::optional<std::string> Token::getQuoteName() const {
    switch (type) {
        case TokenType::QUOTE: return "quote";
        case TokenType::QUASIQUOTE: return "quasiquote";
        case TokenType::UNQUOTE: return "unquote";
        default: return std::nullopt;
    }
}

std::string Token::toString() const {
    switch (type) {
        case TokenType::LEFT_PAREN: return "(LEFT_PAREN)"; break;
        case TokenType::RIGHT_PAREN: return "(RIGHT_PAREN)"; break;
        case TokenType::QUOTE: return "(QUOTE)"; break;
        case TokenType::QUASIQUOTE: return "(QUASIQUOTE)"; break;
        case TokenType::UNQUOTE: return "(UNQUOTE)"; break;
        default: return "(UNKNOWN)";
    }
}

std::unique_ptr<BooleanLiteralToken> BooleanLiteralToken::fromString(const std::string& str) {
    if (str == "#t") {
        return std::make_unique<BooleanLiteralToken>(true);
    } else if (str == "#f") {
        return std::make_unique<BooleanLiteralToken>(false);
    } else {
        return nullptr;
    }
}

std::unique_ptr<BooleanLiteralToken> BooleanLiteralToken::fromChar(char c) {
    if (c == 't') {
        return std::make_unique<BooleanLiteralToken>(true);
    } else if (c == 'f') {
        return std::make_unique<BooleanLiteralToken>(false);
    } else {
        return nullptr;
    }
}

std::string BooleanLiteralToken::toString() const {
    return "(BOOLEAN_LITERAL "s + (value ? "true" : "false") + ")";
}

std::string NumericLiteralToken::toString() const {
    return"(NUMERIC_LITERAL " + std::to_string(value) + ")";
}

std::string StringLiteralToken::toString() const {
    std::ostringstream ss;
    ss << "(STRING_LITERAL " << std::quoted(value) << ")";
    return ss.str();
}

std::string IdentifierToken::toString() const {
    return "(IDENTIFIER " + name + ")";
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    return os << token.toString();
}