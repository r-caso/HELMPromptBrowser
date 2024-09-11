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

static Expression parseQuery(const std::string& querStr, Expression& expr)
{
    AST parsetree;
    auto begin = querStr.cbegin();
    auto end = querStr.cend();
    boost::spirit::qi::parse(begin, end, BooleanGrammar(), parsetree);
    const std::string prettyQueryStr = boost::apply_visitor(ASTFormatter(), parsetree);

    BooleanParser().parse(prettyQueryStr, expr);
    return expr;
}

static std::pair<std::vector<std::string>, std::vector<std::string>> getInclusionExclusionLists(const Expression& expression)
{
    if (isAtomic(expression)) {
        std::vector<std::string> inclusions;
        inclusions.push_back(expression.literal);
        return { inclusions, {} };
    }

    if (isNegation(expression)) {
        std::vector<std::string> exclusions;
        exclusions.push_back(expression.child(0)->literal);
        return { {}, exclusions };
    }

    if (isConjunction(expression)) {
        std::pair<std::vector<std::string>, std::vector<std::string>> lhsQuery = getInclusionExclusionLists(*expression.child(0));
        std::pair<std::vector<std::string>, std::vector<std::string>> rhsQuery = getInclusionExclusionLists(*expression.child(1));

        std::vector<std::string> inclusions;
        std::vector<std::string> exclusions;
        inclusions.reserve(lhsQuery.first.size() + rhsQuery.first.size());
        exclusions.reserve(lhsQuery.second.size() + rhsQuery.second.size());

        std::ranges::merge(lhsQuery.first, rhsQuery.first, std::back_inserter(inclusions));
        std::ranges::merge(lhsQuery.second, rhsQuery.second, std::back_inserter(exclusions));

        return { inclusions, exclusions };
    }
    return {};
}

static std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> getQueries(const Expression& expression)
{
    if (!isDisjunction(expression)) {
        return { getInclusionExclusionLists(expression) };
    }

    if (isConjunction(*expression.child(0))) {
        std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> queries = getQueries(*expression.child(1));
        queries.push_back(getInclusionExclusionLists(*expression.child(0)));
        return queries;
    }
    if (isConjunction(*expression.child(1))) {
        std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> queries = getQueries(*expression.child(0));
        queries.push_back(getInclusionExclusionLists(*expression.child(1)));
        return queries;
    }

    std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> lhsQueries = getQueries(*expression.child(0));
    std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> rhsQueries = getQueries(*expression.child(1));
    std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> queries;
    std::ranges::merge(lhsQueries, rhsQueries, std::back_inserter(queries));
    return queries;
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
