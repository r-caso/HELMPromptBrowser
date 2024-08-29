#include "logic.hpp"

#include <numeric>

bool isAtomic(const Expression& expr)
{
    return expr.op == Operator::NIL;
}
bool isAtomic(std::shared_ptr<Expression> expr)
{
    return expr->op == Operator::NIL;
}
bool isConjunction(const Expression& expr)
{
    return expr.op == Operator::AND;
}
bool isConjunction(std::shared_ptr<Expression> expr)
{
    return expr->op == Operator::AND;
}
bool isDisjunction(const Expression& expr)
{
    return expr.op == Operator::OR;
}
bool isDisjunction(std::shared_ptr < Expression> expr)
{
    return expr->op == Operator::OR;
}
bool isNegation(const Expression& expr)
{
    return expr.op == Operator::NOT;
}
bool isNegation(std::shared_ptr<Expression> expr)
{
    return expr->op == Operator::NOT;
}

static bool noOr(const Expression& expr) {
    if (isAtomic(expr)) {
        return true;
    }
    if (isNegation(expr)) {
        return noOr(*expr.child(0));
    }
    if (isDisjunction(expr)) {
        return false;
    }
    return noOr(*expr.child(0)) && noOr(*expr.child(1));
}
static bool noAndAboveOr(const Expression& expr)
{
    if (isAtomic(expr)) {
        return true;
    }
    if (isNegation(expr)) {
        return noAndAboveOr(*expr.child(0));
    }
    if (isDisjunction(expr)) {
        return noAndAboveOr(*expr.child(0)) && noAndAboveOr(*expr.child(1));
    }
    return noOr(*expr.child(0)) && noOr(*expr.child(1));
}
static Expression distributeAndOr(const Expression& expr1, const Expression& expr2)
{
    if (isDisjunction(expr1)) {
        Expression disjunction(Operator::OR);
        disjunction.addOperands({ distributeAndOr(*expr1.child(0), expr2), distributeAndOr(*expr1.child(1), expr2) });
        return disjunction;
    }
    if (isDisjunction(expr2)) {
        Expression disjunction(Operator::OR);
        disjunction.addOperands({ distributeAndOr(expr1, *expr2.child(0)), distributeAndOr(expr1, *expr2.child(1)) });
        return disjunction;
    }
    Expression conjunction(Operator::AND);
    conjunction.addOperands({ expr1, expr2 });
    return conjunction;
}

bool isNNF(const Expression& expr)
{
    if (isAtomic(expr)) {
        return true;
    }
    if (isNegation(expr)) {
        return isAtomic(expr.child(0));
    }
    return isNNF(*expr.child(0)) && isNNF(*expr.child(1));
}

bool isDNF(const Expression& expr)
{
    return isNNF(expr) && noAndAboveOr(expr);
}

Expression toNNF(const Expression& expr)
{
    if (isAtomic(expr)) {
        return expr;
    }
    if (isNegation(expr)) {
        std::shared_ptr<Expression> scope = expr.child(0);

        if (isAtomic(scope)) {
            return expr;
        }
        if (isNegation(scope)) {
            return toNNF(*scope->child(0));
        }
        if (isDisjunction(scope)) {
            Expression conjunction(Operator::AND);
            Expression lhs(Operator::NOT);
            lhs.addOperand(toNNF(*scope->child(0)));
            Expression rhs(Operator::NOT);
            rhs.addOperand(toNNF(*scope->child(1)));
            conjunction.addOperands({ lhs, rhs });
            return conjunction;
        }
        if (isConjunction(scope)) {
            Expression disjunction(Operator::OR);
            Expression lhs(Operator::NOT);
            lhs.addOperand(toNNF(*scope->child(0)));
            Expression rhs(Operator::NOT);
            rhs.addOperand(toNNF(*scope->child(1)));
            disjunction.addOperands({ lhs, rhs });
            return disjunction;
        }
        return {};
    }
    if (isDisjunction(expr)) {
        Expression disjunction(Operator::OR);
        disjunction.addOperands({ toNNF(*expr.child(0)), toNNF(*expr.child(1)) });
        return disjunction;
    }
    Expression conjunction(Operator::AND);
    conjunction.addOperands({ toNNF(*expr.child(0)), toNNF(*expr.child(1)) });
    return conjunction;
}

Expression NNFtoDNF(const Expression& expr)
{
    if (isAtomic(expr) || isNegation(expr)) {
        return expr;
    }
    if (isDisjunction(expr)) {
        Expression disjunction(Operator::OR);
        disjunction.addOperands({ toDNF(*expr.child(0)), toDNF(*expr.child(1)) });
        return disjunction;
    }
    return distributeAndOr(toDNF(*expr.child(0)), toDNF(*expr.child(1)));
}

Expression toDNF(const Expression& expr)
{
    return NNFtoDNF(toNNF(expr));
}