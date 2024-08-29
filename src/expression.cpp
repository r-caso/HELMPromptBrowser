#include "Expression.hpp"

Expression::Expression()
    : op(Operator::NIL)
    , literal({})
    , children({})
{}

Expression::Expression(Operator op, const std::string& literal, std::vector<std::shared_ptr<Expression>>)
    : op(op), literal(literal)
{}

void Expression::addOperand(std::shared_ptr<Expression> expr)
{
    children.push_back(expr);
}

void Expression::addOperand(const Expression& expr)
{
    children.push_back(std::make_shared<Expression>(expr));
}

void Expression::addOperands(std::vector<Expression> expressions) {
    for (const Expression& expr : expressions) {
        children.push_back(std::make_shared<Expression>(expr));
    }
}

std::string print(const Expression& expr)
{
    if (expr.op == Operator::NIL) {
        return expr.literal;
    }
    else if (expr.op == Operator::AND) {
        return "(" + print(*expr.children.at(0)) + " & " + print(*expr.children.at(1)) + ")";
    }
    else if (expr.op == Operator::OR) {
        return "(" + print(*expr.children.at(0)) + " | " + print(*expr.children.at(1)) + ")";
    }
    else if (expr.op == Operator::NOT) {
        return "!" + print(*expr.children.at(0));
    }

    return "";
}

int Expression::childCount() const
{
    return children.size();
}

std::shared_ptr<Expression> Expression::child(int pos) const
{
    return children.at(pos);
}