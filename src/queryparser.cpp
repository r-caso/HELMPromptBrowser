#include "queryparser.hpp"

#include <QList>
#include <QPair>
#include <QString>

#include "booleanparser.hpp"
#include "expression.hpp"
#include "logic.hpp"

bool checkQuery(const QString& query)
{
    Expression expr;
    return BooleanParser().parse(query, expr);
}

namespace {
    QPair<QList<QString>, QList<QString>> getQueryLists(const Expression& expression)
    {
        if (isAtomic(expression)) {
            QList<QString> inclusions{ expression.literal() };
            return { inclusions, {} };
        }
        if (isNegation(expression)) {
            QList<QString> exclusions{ expression.scope().literal() };
            return { {}, exclusions };
        }
        if (isConjunction(expression)) {
            const auto& [lhs_inclusions, lhs_exclusions] = getQueryLists(expression.lhs());
            const auto& [rhs_inclusions, rhs_exclusions] = getQueryLists(expression.rhs());

            QList<QString> inclusions;
            inclusions.reserve(lhs_inclusions.size() + rhs_inclusions.size());

            QList<QString> exclusions;
            exclusions.reserve(lhs_exclusions.size() + rhs_exclusions.size());

            std::ranges::merge(lhs_inclusions, rhs_inclusions, std::back_inserter(inclusions));
            std::ranges::merge(lhs_exclusions, rhs_exclusions, std::back_inserter(exclusions));

            return { inclusions, exclusions };
        }
        return {};
    }

    QList<QPair<QList<QString>, QList<QString>>> getQueries(const Expression& expression)
    {
        if (!isDisjunction(expression)) {
            return { getQueryLists(expression) };
        }

        if (isConjunction(expression.lhs())) {
            QList<QPair<QList<QString>, QList<QString>>> queries = getQueries(expression.rhs());
            queries.push_back(getQueryLists(expression.lhs()));
            return queries;
        }
        if (isConjunction(expression.rhs())) {
            QList<QPair<QList<QString>, QList<QString>>> queries = getQueries(expression.lhs());
            queries.push_back(getQueryLists(expression.rhs()));
            return queries;
        }

        QList<QPair<QList<QString>, QList<QString>>> lhs_queries = getQueries(expression.lhs());
        QList<QPair<QList<QString>, QList<QString>>> rhs_queries = getQueries(expression.rhs());
        QList<QPair<QList<QString>, QList<QString>>> queries;
        std::ranges::merge(lhs_queries, rhs_queries, std::back_inserter(queries));
        return queries;
    }
}

QList<QPair<QList<QString>, QList<QString>>> getQueries(const QString& queryStr)
{
    Expression expr;
    BooleanParser().parse(queryStr, expr);
    return getQueries(toDNF(expr));
}
