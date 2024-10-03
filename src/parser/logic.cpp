#include "logic.hpp"

bool isAtomic(const Expression& expr)
{
    return expr.op() == Operator::NIL;
}

bool isConjunction(const Expression& expr)
{
    return expr.op() == Operator::AND;
}

bool isDisjunction(const Expression& expr)
{
    return expr.op() == Operator::OR;
}

bool isNegation(const Expression& expr)
{
    return expr.op() == Operator::NOT;
}

namespace {
    bool noOr(const Expression& expr) {
        if (isAtomic(expr)) {
            return true;
        }
        if (isNegation(expr)) {
            return noOr(expr.scope());
        }
        if (isDisjunction(expr)) {
            return false;
        }
        return noOr(expr.lhs()) && noOr(expr.rhs());
    }

    bool noAndAboveOr(const Expression& expr)
    {
        if (isAtomic(expr)) {
            return true;
        }
        if (isNegation(expr)) {
            return noAndAboveOr(expr.scope());
        }
        if (isDisjunction(expr)) {
            return noAndAboveOr(expr.lhs()) && noAndAboveOr(expr.rhs());
        }
        return noOr(expr.lhs()) && noOr(expr.rhs());
    }

    Expression distributeAndOr(const Expression& expr1, const Expression& expr2)
    {
        if (isDisjunction(expr1)) {
            return Expression(Operator::OR, "", { distributeAndOr(expr1.lhs(), expr2), distributeAndOr(expr1.rhs(), expr2) });
        }
        if (isDisjunction(expr2)) {
            return Expression(Operator::OR, "", { distributeAndOr(expr1, expr2.lhs()), distributeAndOr(expr1, expr2.rhs()) });
        }
        return Expression(Operator::AND, "", { expr1, expr2 });
    }
    } // namespace

bool isDNF(const Expression& expr)
{
    return isNNF(expr) && noAndAboveOr(expr);
}

bool isNNF(const Expression& expr)
{
    if (isAtomic(expr)) {
        return true;
    }
    if (isNegation(expr)) {
        return isAtomic(expr.scope());
    }
    return isNNF(expr.lhs()) && isNNF(expr.rhs());
}

Expression NNFtoDNF(const Expression& expr)
{
    if (isAtomic(expr) || isNegation(expr)) {
        return expr;
    }
    if (isDisjunction(expr)) {
        return Expression(Operator::OR, "", { toDNF(expr.lhs()), toDNF(expr.rhs()) });
    }
    return distributeAndOr(toDNF(expr.lhs()), toDNF(expr.rhs()));
}

Expression toDNF(const Expression& expr)
{
    return NNFtoDNF(toNNF(expr));
}

Expression toNNF(const Expression& expr)
{
    if (isAtomic(expr)) {
        return expr;
    }
    if (isNegation(expr)) {
        const Expression& scope = expr.scope();

        if (isAtomic(scope)) {
            return expr;
        }
        if (isNegation(scope)) {
            return toNNF(scope.scope());
        }
        if (isDisjunction(scope)) {
            const Expression lhs = isNegation(toNNF(scope.lhs())) ? toNNF(scope.lhs()).scope() : Expression(Operator::NOT, "", { toNNF(scope.lhs()) });
            const Expression rhs = isNegation(toNNF(scope.rhs())) ? toNNF(scope.rhs()).scope() : Expression(Operator::NOT, "", { toNNF(scope.rhs()) });
            return Expression(Operator::AND, "", { lhs, rhs });
        }
        if (isConjunction(scope)) {
            const Expression lhs = isNegation(toNNF(scope.lhs())) ? toNNF(scope.lhs()).scope() : Expression(Operator::NOT, "", { toNNF(scope.lhs()) });
            const Expression rhs = isNegation(toNNF(scope.rhs())) ? toNNF(scope.rhs()).scope() : Expression(Operator::NOT, "", { toNNF(scope.rhs()) });
            return Expression(Operator::OR, "", { lhs, rhs });
        }
        return {};
    }
    if (isDisjunction(expr)) {
        return Expression(Operator::OR, "", { toNNF(expr.lhs()), toNNF(expr.rhs()) });
    }
    return Expression(Operator::AND, "", { toNNF(expr.lhs()), toNNF(expr.rhs()) });
}
