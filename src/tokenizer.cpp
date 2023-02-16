#include "./tokenizer.h"

#include <cctype>
#include <set>

#include "./error.h"

const std::set<char> TOKEN_END{'(', ')', '\'', '`', ',', '"'};

std::unique_ptr<Token> Tokenizer::nextToken(int& pos) {
    while (pos < input.size()) {
        auto c = input[pos];
        if (std::isspace(c)) {
            pos++;
        } else if (auto token = Token::fromChar(c)) {
            pos++;
            return token;
        } else if (c == '#') {
            if (auto result = BooleanLiteralToken::fromChar(input[pos + 1])) {
                pos += 2;
                return result;
            } else {
                throw SyntaxError("Unexpected character after #");
            }
        } else if (c == '"') {
            std::string string;
            pos++;
            while (pos < input.size()) {
                if (input[pos] == '"') {
                    pos++;
                    return std::make_unique<StringLiteralToken>(string);
                } else if (input[pos] == '\\') {
                    if (pos + 1 >= input.size()) {
                        throw SyntaxError("Unexpected end of string literal");
                    }
                    auto next = input[pos + 1];
                    if (next == 'n') {
                        string += '\n';
                    } else {
                        string += next;
                    }
                    pos += 2;
                } else {
                    string += input[pos];
                    pos++;
                }
            }
            throw SyntaxError("Unexpected end of string literal");
        } else {
            int start = pos;
            do {
                pos++;
            } while (pos < input.size() && !std::isspace(input[pos]) &&
                     !TOKEN_END.contains(input[pos]));
            auto text = input.substr(start, pos - start);
            if (std::isdigit(text[0])) {
                return std::make_unique<NumericLiteralToken>(std::stoi(text));
            } else {
                return std::make_unique<IdentifierToken>(text);
            }
        }
    }
    return nullptr;
}

std::deque<std::unique_ptr<Token>> Tokenizer::tokenize() {
    std::deque<std::unique_ptr<Token>> tokens;
    int pos = 0;
    while (true) {
        auto token = nextToken(pos);
        if (!token) {
            break;
        }
        // std::clog << *token << std::endl;
        tokens.push_back(std::move(token));
    }
    return tokens;
}