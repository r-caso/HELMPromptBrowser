#include "booleanparser.hpp"

#include <cctype>
#include <memory>
#include <stack>

BooleanParser::BooleanParser()
    : m_Index(0), m_Sym(TokenType::START_SYMBOL), m_TokenList({})
{}


bool BooleanParser::parse(const std::string& formula, Expression& expr)
{
    tokenize(formula);
    return sentence(expr);
}

void BooleanParser::advance()
{
    if (!(m_Sym == TokenType::END_SYMBOL))
    {
        m_Index++;
        m_Sym = m_TokenList.at(m_Index).second;
    }
}

bool BooleanParser::expect(TokenType type)
{
    if (m_Sym == type)
    {
        advance();
        return true;
    }
    return false;
}

bool BooleanParser::expression(Expression& expr)
{
    if (expect(TokenType::IDENTIFIER)) {
        expr = Expression(Operator::NIL, m_TokenList.at(m_Index - 1).first);
        expr.addOperand(expr);
        return true;
    }

    if (expect(TokenType::NOT)) {
        bool const result = expression(expr);
        Expression new_expr = Expression(Operator::NOT);
        new_expr.addOperand(std::make_shared<Expression>(expr));
        expr = new_expr;
        return result;
    }

    if (expect(TokenType::LPAREN)) {
        Expression new_expr;
        bool const lhs_is_expression = expression(expr);
        if (!lhs_is_expression) {
            return false;
        }
        
        new_expr.addOperand(std::make_shared<Expression>(expr));

        bool const main_connective_is_binary_operator = expect(TokenType::AND)
                                                        || expect(TokenType::OR);
        if (!main_connective_is_binary_operator) {
            return false;
        }
        
        new_expr.op = (m_TokenList.at(m_Index - 1).second == TokenType::AND) ? Operator::AND : Operator::OR;

        bool const rhs_is_expression = expression(expr);
        if (!rhs_is_expression) {
            return false;
        }

        new_expr.addOperand(std::make_shared<Expression>(expr));

        if (!expect(TokenType::RPAREN)) {
            return false;
        }

        expr = new_expr;

        return true;
    }

    return false;
}

bool BooleanParser::sentence(Expression& expr)
{
    expect(TokenType::START_SYMBOL);

    if (!expression(expr)) {
        return false;
    }

    return expect(TokenType::END_SYMBOL);
}

void BooleanParser::tokenize(const std::string& formula)
{
    m_TokenList.push_back({ TokenTypeName.at(TokenType::START_SYMBOL), TokenType::START_SYMBOL });

    std::string ident{};

    std::stack<char> symbol_stack;

    for (char const c : formula) {
        if (!isalpha(c) && !ident.empty()) {
            if (!symbol_stack.empty()) {
                continue;
            }
            m_TokenList.push_back({ ident, TokenType::IDENTIFIER });
            ident.clear();
        }
        if (isalpha(c)) {
            ident.push_back(c);
        }
        else if (c == ' ') {
            if (symbol_stack.empty()) {
                continue;
            }
            ident.push_back(c);
        } else if (c == '"') {
            if (symbol_stack.empty()) {
                symbol_stack.push(c);
            }
            else {
                symbol_stack.pop();
            }
        } else if (c == '(') {
            m_TokenList.push_back({ std::string(1, c), TokenType::LPAREN });
        } else if (c == ')') {
            m_TokenList.push_back({ std::string(1, c), TokenType::RPAREN });
        } else if (c == '!') {
            m_TokenList.push_back({ std::string(1, c), TokenType::NOT });
        } else if (c == '|') {
            m_TokenList.push_back({ std::string(1, c), TokenType::OR });
        } else if (c == '&') {
            m_TokenList.push_back({ std::string(1, c), TokenType::AND });
        } else {
            m_TokenList.push_back({ std::string(1, c), TokenType::ILLEGAL });
        }
    }

    if (!ident.empty()) {
        m_TokenList.push_back({ ident, TokenType::IDENTIFIER });
    }

    m_TokenList.push_back({ TokenTypeName.at(TokenType::END_SYMBOL), TokenType::END_SYMBOL });
}
