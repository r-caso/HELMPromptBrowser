#include "expression.hpp"

Expression::Expression()
    : m_Operator(Operator::NIL)
    , m_Literal({})
    , m_Children({})
{}

Expression::Expression(Operator op, const std::string& literal, const std::vector<Expression>& children)
    : m_Operator(op), m_Literal(literal)
{
    for (const auto& child : children) {
        m_Children.push_back(std::make_shared<Expression>(child));
    }
}

void Expression::setOperator(Operator op)
{
    m_Operator = op;
}

void Expression::addOperand(const Expression& expr)
{
    m_Children.push_back(std::make_shared<Expression>(expr));
}

void Expression::addOperands(const std::vector<Expression>& expressions)
{
    for (const Expression& expr : expressions) {
        m_Children.push_back(std::make_shared<Expression>(expr));
    }
}

const Expression& Expression::lhs() const
{
    return *m_Children.at(0);
}

const Expression& Expression::rhs() const
{
    return *m_Children.at(1);
}

const Expression& Expression::scope() const
{
    return *m_Children.at(0);
}

const std::string& Expression::literal() const
{
    return m_Literal;
}

Operator Expression::op() const
{
    return m_Operator;
}

std::string print(const Expression& expr)
{
    if (expr.op() == Operator::NIL) {
        return expr.literal();
    }
    if (expr.op() == Operator::AND) {
        return "(" + print(expr.lhs()) + " & " + print(expr.rhs()) + ")";
    }
    if (expr.op() == Operator::OR) {
        return "(" + print(expr.lhs()) + " | " + print(expr.rhs()) + ")";
    }
    if (expr.op() == Operator::NOT) {
        return "!" + print(expr.scope());
    }
    return "";
}
