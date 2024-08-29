#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "expression.hpp"

enum class TokenType { START_SYMBOL, END_SYMBOL, LPAREN, RPAREN, AND, OR, NOT, IDENTIFIER, ILLEGAL };

class BooleanParser
{
public:
    BooleanParser();

    bool parse(const std::string& formula, Expression& expr);

private:
    void advance();
    bool expect(TokenType s);
    bool sentence(Expression& expr);
    bool expression(Expression& expr);

    void tokenize(const std::string& formula);

private:
    int m_Index;
    TokenType m_Sym;
    std::vector<std::pair<std::string, TokenType>> m_TokenList;

    const std::unordered_map<TokenType, std::string> TokenTypeName = {
        { TokenType::START_SYMBOL, "<S>" },
        { TokenType::END_SYMBOL, "<E>" },
        { TokenType::LPAREN, "lparen" },
        { TokenType::RPAREN, "rparen" },
        { TokenType::IDENTIFIER, "ident" },
        { TokenType::NOT, "not" },
        { TokenType::AND, "and" },
        { TokenType::OR, "or" },
        { TokenType::ILLEGAL, "illegal" },
    };
};

