#include "./token.h"

#include <iomanip>

std::unique_ptr<Token> Token::fromChar(char c) {
    switch (c) {
        case '(': return std::make_unique<Token>(TokenType::LEFT_PAREN);
        case ')': return std::make_unique<Token>(TokenType::RIGHT_PAREN);
        case '\'': return std::make_unique<Token>(TokenType::QUOTE);
        case '`': return std::make_unique<Token>(TokenType::QUASIQUOTE);
        case ',': return std::make_unique<Token>(TokenType::UNQUOTE);
        default: return nullptr;
    }
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

std::ostream& operator<<(std::ostream& os, const Token& token) {
    switch (token.getType()) {
        case TokenType::LEFT_PAREN: os << "(LEFT_PAREN)"; break;
        case TokenType::RIGHT_PAREN: os << "(RIGHT_PAREN)"; break;
        case TokenType::QUOTE: os << "(QUOTE)"; break;
        case TokenType::QUASIQUOTE: os << "(QUASIQUOTE)"; break;
        case TokenType::UNQUOTE: os << "(UNQUOTE)"; break;
        case TokenType::BOOLEAN_LITERAL:
            os << "(BOOLEAN_LITERAL " << std::boolalpha
               << static_cast<const BooleanLiteralToken&>(token).getValue() << ")";
            break;
        case TokenType::NUMERIC_LITERAL:
            os << "(NUMERIC_LITERAL " << static_cast<const NumericLiteralToken&>(token).getValue()
               << ")";
            break;
        case TokenType::STRING_LITERAL:
            os << "(STRING_LITERAL "
               << std::quoted(static_cast<const StringLiteralToken&>(token).getValue()) << ")";
            break;
        case TokenType::IDENTIFIER:
            os << "(IDENTIFIER " << static_cast<const IdentifierToken&>(token).getName() << ")";
            break;
        default: os << "(UNKNOWN)"; break;
    }
    return os;
}