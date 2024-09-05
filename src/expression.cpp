#include "expression.hpp"

Expression::Expression()
    : op(Operator::NIL)
    , literal({})
    , children({})
{}

Expression::Expression(Operator op, const std::string& literal, const std::vector<std::shared_ptr<Expression>>& children)
    : op(op), literal(literal), children(children)
{}

void Expression::addOperand(std::shared_ptr<Expression> expr)
{
    children.push_back(expr);
}

void Expression::addOperand(const Expression& expr)
{
    children.push_back(std::make_shared<Expression>(expr));
}

void Expression::addOperands(const std::vector<Expression>& expressions)
{
    for (const Expression& expr : expressions) {
        children.push_back(std::make_shared<Expression>(expr));
    }
}

std::string print(const Expression& expr)
{
    if (expr.op == Operator::NIL) {
        return expr.literal;
    }
    if (expr.op == Operator::AND) {
        return "(" + print(*expr.children.at(0)) + " & " + print(*expr.children.at(1)) + ")";
    }
    if (expr.op == Operator::OR) {
        return "(" + print(*expr.children.at(0)) + " | " + print(*expr.children.at(1)) + ")";
    }
    if (expr.op == Operator::NOT) {
        return "!" + print(*expr.children.at(0));
    }

    return "";
}

size_t Expression::childCount() const
{
    return children.size();
}

const Expression* Expression::child(int pos) const
{
    return children.at(pos).get();
}
