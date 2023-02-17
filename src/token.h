#ifndef TOKEN_H
#define TOKEN_H

#include <memory>
#include <optional>
#include <ostream>
#include <string>


enum class TokenType {
    LEFT_PAREN,
    RIGHT_PAREN,
    QUOTE,
    QUASIQUOTE,
    UNQUOTE,
    BOOLEAN_LITERAL,
    NUMERIC_LITERAL,
    STRING_LITERAL,
    IDENTIFIER,
};

class Token {
private:
    TokenType type;

protected:
    Token(TokenType type) : type{type} {}

public:
    virtual ~Token() = default;

    static std::unique_ptr<Token> fromChar(char c);

    TokenType getType() const {
        return type;
    }
    bool isDot() const;
    std::optional<std::string> getQuoteName() const;
    virtual std::string toString() const;
};

class BooleanLiteralToken : public Token {
private:
    bool value;

public:
    BooleanLiteralToken(bool value) : Token(TokenType::BOOLEAN_LITERAL), value{value} {}

    static std::unique_ptr<BooleanLiteralToken> fromString(const std::string& str);
    static std::unique_ptr<BooleanLiteralToken> fromChar(char c);

    bool getValue() const {
        return value;
    }
    std::string toString() const override;
};

class NumericLiteralToken : public Token {
private:
    double value;

public:
    NumericLiteralToken(double value) : Token(TokenType::NUMERIC_LITERAL), value{value} {}

    double getValue() const {
        return value;
    }
    std::string toString() const override;
};

class StringLiteralToken : public Token {
private:
    std::string value;

public:
    StringLiteralToken(const std::string& value) : Token(TokenType::STRING_LITERAL), value{value} {}

    const std::string& getValue() const {
        return value;
    }
    std::string toString() const override;
};

class IdentifierToken : public Token {
private:
    std::string name;

public:
    IdentifierToken(const std::string& name) : Token(TokenType::IDENTIFIER), name{name} {}

    const std::string& getName() const {
        return name;
    }
    std::string toString() const override;
};

std::ostream& operator<<(std::ostream& os, const Token& token);

#endif