#include "expression.hpp"

Expression::Expression()
    : m_Operator(Operator::NIL)
    , m_Literal(QString())
    , m_Children(QList<std::shared_ptr<Expression>>())
{}

Expression::Expression(Operator op, const QString& literal, const QList<Expression>& children)
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

void Expression::addOperands(const QList<Expression>& expressions)
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

const QString& Expression::literal() const
{
    return m_Literal;
}

Operator Expression::op() const
{
    return m_Operator;
}

void Expression::clear()
{
    m_Operator = Operator::NIL;
    m_Literal.clear();
    m_Children.clear();
}
