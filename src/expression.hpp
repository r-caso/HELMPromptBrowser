#pragma once

#include <memory>
#include <string>
#include <vector>

enum class Operator { NOT, AND, OR, NIL };

struct Conjunction;
struct Disjunction;
struct Negation;

class Expression
{
public:
    Expression();
    Expression(Operator op, const std::string& literal = "", std::vector<std::shared_ptr<Expression>> = {});
    void addOperand(std::shared_ptr<Expression> expr);
    void addOperand(const Expression& expr);
    void addOperands(std::vector<Expression> expressions);
    int childCount() const;
    std::shared_ptr<Expression> child(int pos) const;

    Operator op;
    std::string literal;
    std::vector<std::shared_ptr<Expression>> children;
};

std::string print(const Expression& expr);