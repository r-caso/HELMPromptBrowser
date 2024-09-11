#pragma once

#include <memory>
#include <string>
#include <vector>

enum class Operator : uint8_t { NOT, AND, OR, NIL };

struct Conjunction;
struct Disjunction;
struct Negation;

class Expression
{
public:
    Expression();
    Expression(Operator op, const std::string& literal = "", const std::vector<Expression> & = {});
    void setOperator(Operator op);
    void addOperand(const Expression& expr);
    void addOperands(const std::vector<Expression>& expressions);
    const Expression& lhs() const;
    const Expression& rhs() const;
    const Expression& scope() const;
    const std::string& literal() const;
    Operator op() const;

private:
    Operator m_Operator;
    std::string m_Literal;
    std::vector<std::shared_ptr<Expression>> m_Children;
};

std::string print(const Expression& expr);
