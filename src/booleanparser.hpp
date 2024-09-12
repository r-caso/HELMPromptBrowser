#pragma once

#include <QString>
#include <QMap>
#include <QPair>
#include <QList>

#include "expression.hpp"

enum class TokenType : uint8_t { START_SYMBOL, END_SYMBOL, LPAREN, RPAREN, AND, OR, NOT, IDENTIFIER, ILLEGAL };

class BooleanParser {
public:
    BooleanParser();

    bool parse(const QString& formula, Expression& expr);

private:
    void advance();
    bool match(TokenType type);
    bool sentence(Expression& expr);
    bool disjunction(Expression& expr);
    bool conjunction(Expression& expr);
    bool negation(Expression& expr);

    void tokenize(const QString& formula);

    int m_Index;
    TokenType m_Sym;
    QList<QPair<QString, TokenType>> m_TokenList;

    inline static const QMap<TokenType, QString> TokenTypeName = {
        { TokenType::START_SYMBOL, QString("<S>") },
        { TokenType::END_SYMBOL, QString("<E>") },
        { TokenType::LPAREN, QString("lparen") },
        { TokenType::RPAREN, QString("rparen") },
        { TokenType::NOT, QString("not") },
        { TokenType::AND, QString("and") },
        { TokenType::OR, QString("or") },
        { TokenType::ILLEGAL, QString("illegal") },
    };
};
