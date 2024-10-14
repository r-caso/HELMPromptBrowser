#include "booleanparser.hpp"

#include <QChar>
#include <QStack>

BooleanParser::BooleanParser()
    : m_Index(0), m_Sym(TokenType::START_SYMBOL), m_TokenList({})
{}

bool BooleanParser::parse(const QString& formula, Expression& expr)
{
    expr.clear();
    tokenize(formula);
    return sentence(expr);
}

bool BooleanParser::check(const QString& formula)
{
    Expression expr;
    tokenize(formula);
    return sentence(expr);
}

namespace {
    bool isPunctuation(QChar c)
    {
        return c == '(' || c == ')' || c == '"' || c == ' ';
    }
    bool isOperator(QChar c)
    {
        return c == '!' || c == '&' || c == '|';
    }
    bool isValidQueryChar(QChar c)
    {
        return !isPunctuation(c) && !isOperator(c);
    }
    } // namespace

void BooleanParser::tokenize(const QString& formula)
{
    m_TokenList.clear();
    m_TokenList.push_back({ TokenTypeName[TokenType::START_SYMBOL], TokenType::START_SYMBOL });
    QString ident{};
    QStack<QChar> symbol_stack;

    for (QChar const c : formula) {
        if (!isValidQueryChar(c) && !ident.isEmpty()) {
            if (!symbol_stack.empty()) {
                if (c != '"') {
                    ident.push_back(c);
                }
                continue;
            }
            m_TokenList.push_back({ ident, TokenType::IDENTIFIER });
            ident.clear();
        }
        if (isValidQueryChar(c)) {
            ident.push_back(c);
        }
        else if (c == ' ') {
            if (symbol_stack.empty()) {
                continue;
            }
            ident.push_back(c);
        }
        else if (c == '"') {
            if (symbol_stack.empty()) {
                symbol_stack.push(c);
            }
            else {
                symbol_stack.pop();
            }
        }
        else if (c == '(') {
            m_TokenList.push_back({QString(c), TokenType::LPAREN });
        }
        else if (c == ')') {
            m_TokenList.push_back({QString(c), TokenType::RPAREN });
        }
        else if (c == '!') {
            m_TokenList.push_back({QString(c), TokenType::NOT });
        }
        else if (c == '|') {
            m_TokenList.push_back({QString(c), TokenType::OR });
        }
        else if (c == '&') {
            m_TokenList.push_back({QString(c), TokenType::AND });
        }
        else {
            m_TokenList.push_back({QString(c), TokenType::ILLEGAL });
        }
    }

    if (!ident.isEmpty()) {
        m_TokenList.push_back({ ident, TokenType::IDENTIFIER });
    }

    m_TokenList.push_back({ TokenTypeName[TokenType::END_SYMBOL], TokenType::END_SYMBOL });
}

void BooleanParser::advance()
{
    if (m_Sym != TokenType::END_SYMBOL) {
        m_Index++;
        m_Sym = m_TokenList.at(m_Index).second;
    }
}

bool BooleanParser::match(TokenType type)
{
    if (m_Sym != type) {
        return false;
    }
    advance();
    return true;
}

bool BooleanParser::sentence(Expression& expr)
{
    if (!match(TokenType::START_SYMBOL)) {
        return false;
    }
    if (!disjunction(expr)) {
        return false;
    }
    return match(TokenType::END_SYMBOL);
}

bool BooleanParser::disjunction(Expression& expr)
{
    if (!conjunction(expr)) {
        return false;
    }
    while (match(TokenType::OR)) {
        Expression new_expr(Operator::OR, "", { expr });
        const bool result = conjunction(expr);
        if (result) {
            new_expr.addOperand(expr);
            expr = new_expr;
            continue;
        }
        return result;
    }
    return true;
}

bool BooleanParser::conjunction(Expression& expr)
{
    if (!negation(expr)) {
        return false;
    }
    while (match(TokenType::AND)) {
        Expression new_expr(Operator::AND, "", { expr });
        const bool result = negation(expr);
        if (result) {
            new_expr.addOperand(expr);
            expr = new_expr;
            continue;
        }
        return result;
    }
    return true;
}

bool BooleanParser::negation(Expression& expr)
{
    if (match(TokenType::IDENTIFIER)) {
        expr = Expression(Operator::NIL, m_TokenList.at(m_Index - 1).first);
        expr.addOperand(expr);
        return true;
    }
    if (match(TokenType::NOT)) {
        if (!negation(expr)) {
            return false;
        }
        Expression new_expr = Expression(Operator::NOT);
        new_expr.addOperand(expr);
        expr = new_expr;
        return true;
    }
    if (match(TokenType::LPAREN)) {
        if (!disjunction(expr)) {
            return false;
        }
        return match(TokenType::RPAREN);
    }
    return false;
}
