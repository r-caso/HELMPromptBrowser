#include "queryparser.hpp"

#include <algorithm>

#include "booleanparser.hpp"
#include "expression.hpp"
#include "logic.hpp"

bool checkQuery(const QString& query)
{
    if (query.isEmpty()) {
        return true;
    }
    return BooleanParser().check(query);
}

namespace {
    QPair<QStringList, QStringList> getQueryLists(const Expression& expression)
    {
        if (isAtomic(expression)) {
            QStringList const inclusions{expression.literal()};
            return { inclusions, {} };
        }
        if (isNegation(expression)) {
            QStringList const exclusions{expression.scope().literal()};
            return { {}, exclusions };
        }
        if (isConjunction(expression)) {
            const auto& [lhs_inclusions, lhs_exclusions] = getQueryLists(expression.lhs());
            const auto& [rhs_inclusions, rhs_exclusions] = getQueryLists(expression.rhs());

            QStringList inclusions;
            inclusions.reserve(lhs_inclusions.size() + rhs_inclusions.size());

            QStringList exclusions;
            exclusions.reserve(lhs_exclusions.size() + rhs_exclusions.size());

            std::ranges::merge(lhs_inclusions, rhs_inclusions, std::back_inserter(inclusions));
            std::ranges::merge(lhs_exclusions, rhs_exclusions, std::back_inserter(exclusions));

            return { inclusions, exclusions };
        }
        return {};
    }

    QList<QPair<QStringList, QStringList>> getQueries(const Expression& expression)
    {
        if (!isDisjunction(expression)) {
            return { getQueryLists(expression) };
        }

        if (isConjunction(expression.lhs())) {
            QList<QPair<QStringList, QStringList>> queries = getQueries(expression.rhs());
            queries.push_back(getQueryLists(expression.lhs()));
            return queries;
        }
        if (isConjunction(expression.rhs())) {
            QList<QPair<QStringList, QStringList>> queries = getQueries(expression.lhs());
            queries.push_back(getQueryLists(expression.rhs()));
            return queries;
        }

        QList<QPair<QStringList, QStringList>> lhs_queries = getQueries(expression.lhs());
        QList<QPair<QStringList, QStringList>> rhs_queries = getQueries(expression.rhs());
        QList<QPair<QStringList, QStringList>> queries;
        std::ranges::merge(lhs_queries, rhs_queries, std::back_inserter(queries));
        return queries;
    }
    } // namespace

QList<QPair<QStringList, QStringList>> getQueries(const QString& queryStr)
{
    if (queryStr.isEmpty()) {
        return { {}, {} };
    }

    Expression expr;
    BooleanParser().parse(queryStr, expr);
    return getQueries(toDNF(expr));
}
