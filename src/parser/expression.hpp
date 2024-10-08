#pragma once

#include <memory>

#include <QList>
#include <QString>

enum class Operator : uint8_t { NOT, AND, OR, NIL };

struct Conjunction;
struct Disjunction;
struct Negation;

class Expression
{
public:
    Expression() = default;
    Expression(Operator op, QString literal = "", const QList<Expression>& = {});
    void setOperator(Operator op);
    void addOperand(const Expression& expr);
    void addOperands(const QList<Expression>& expressions);
    const Expression& lhs() const;
    const Expression& rhs() const;
    const Expression& scope() const;
    const QString& literal() const;
    Operator op() const;
    void clear();

private:
    Operator m_Operator = Operator::NIL;
    QString m_Literal = "";
    QList<std::shared_ptr<Expression>> m_Children = {};
};
