#include "queryparser.hpp"

#include <QList>
#include <QPair>
#include <QString>

#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>

#include "booleangrammar.hpp"
#include "booleanparser.hpp"
#include "logic.hpp"

bool checkQuery(const QString& query)
{
    return checkQuery(query.toStdString());
}

namespace {
    Expression parseQuery(const std::string& querStr, Expression& expr)
    {
        AST parsetree;
        auto begin = querStr.cbegin();
        auto end = querStr.cend();
        boost::spirit::qi::parse(begin, end, BooleanGrammar(), parsetree);
        const std::string prettyQueryStr = boost::apply_visitor(ASTFormatter(), parsetree);

        BooleanParser().parse(prettyQueryStr, expr);
        return expr;
    }

    std::pair<std::vector<std::string>, std::vector<std::string>> getQueryLists(const Expression& expression)
    {
        if (isAtomic(expression)) {
            std::vector<std::string> inclusions{ expression.literal() };
            return { inclusions, {} };
        }
        if (isNegation(expression)) {
            std::vector<std::string> exclusions{ expression.scope().literal() };
            return { {}, exclusions };
        }
        if (isConjunction(expression)) {
            const auto& [lhs_inclusions, lhs_exclusions] = getQueryLists(expression.lhs());
            const auto& [rhs_inclusions, rhs_exclusions] = getQueryLists(expression.rhs());

            std::vector<std::string> inclusions;
            inclusions.reserve(lhs_inclusions.size() + rhs_inclusions.size());

            std::vector<std::string> exclusions;
            exclusions.reserve(lhs_exclusions.size() + rhs_exclusions.size());

            std::ranges::merge(lhs_inclusions, rhs_inclusions, std::back_inserter(inclusions));
            std::ranges::merge(lhs_exclusions, rhs_exclusions, std::back_inserter(exclusions));

            return { inclusions, exclusions };
        }
        return {};
    }

    std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> getQueries(const Expression& expression)
    {
        if (!isDisjunction(expression)) {
            return { getQueryLists(expression) };
        }

        if (isConjunction(expression.lhs())) {
            std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> queries = getQueries(expression.rhs());
            queries.push_back(getQueryLists(expression.lhs()));
            return queries;
        }
        if (isConjunction(expression.rhs())) {
            std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> queries = getQueries(expression.lhs());
            queries.push_back(getQueryLists(expression.rhs()));
            return queries;
        }

        std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> lhs_queries = getQueries(expression.lhs());
        std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> rhs_queries = getQueries(expression.rhs());
        std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> queries;
        std::ranges::merge(lhs_queries, rhs_queries, std::back_inserter(queries));
        return queries;
    }
}

QList<QPair<QList<QString>, QList<QString>>> getQueries(const QString& queryStr)
{
    Expression expr;
    parseQuery(queryStr.toStdString(), expr);
    const auto queries = getQueries(toDNF(expr));

    QList<QPair<QList<QString>, QList<QString>>> qQueries;

    for (const auto& queryPair : queries) {
        const auto& [inclusions, exclusions] = queryPair;
        QList<QString> qInclusions;
        QList<QString> qExclusions;
        for (const auto& str : inclusions) {
            qInclusions.push_back(QString::fromStdString(str));
        }
        for (const auto& str : exclusions) {
            qExclusions.push_back(QString::fromStdString(str));
        }
        QPair<QList<QString>, QList<QString>> const qQueryPair({qInclusions, qExclusions});
        qQueries.push_back(qQueryPair);
    }

    return qQueries;
}
