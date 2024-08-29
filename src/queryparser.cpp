#include "queryparser.hpp"

#include <QList>
#include <QPair>
#include <QString>

#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>

#include "booleangrammar.hpp"
#include "booleanParser.hpp"
#include "logic.hpp"

bool checkQuery(const QString& query)
{
    return checkQuery(query.toStdString());
}

static Expression parseQuery(const std::string& query_str, Expression& expr)
{
    AST parsetree;
    auto begin = query_str.cbegin();
    auto end = query_str.cend();
    boost::spirit::qi::parse(begin, end, BooleanGrammar(), parsetree);
    const std::string pretty_query_str = boost::apply_visitor(ASTFormatter(), parsetree);

    BooleanParser().parse(pretty_query_str, expr);
    return expr;
}

static std::pair<std::vector<std::string>, std::vector<std::string>> get_inclusion_exclusion_lists(const Expression& expression)
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
        std::pair<std::vector<std::string>, std::vector<std::string>> lhs_query = get_inclusion_exclusion_lists(*expression.child(0));
        std::pair<std::vector<std::string>, std::vector<std::string>> rhs_query = get_inclusion_exclusion_lists(*expression.child(1));

        std::vector<std::string> inclusions;
        std::vector<std::string> exclusions;
        inclusions.reserve(lhs_query.first.size() + rhs_query.first.size());
        exclusions.reserve(lhs_query.second.size() + rhs_query.second.size());

        std::ranges::merge(lhs_query.first, rhs_query.first, std::back_inserter(inclusions));
        std::ranges::merge(lhs_query.second, rhs_query.second, std::back_inserter(exclusions));

        return { inclusions, exclusions };
    }
    return {};
}

static std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> getQueries(const Expression& expression)
{
    if (!isDisjunction(expression)) {
        return { get_inclusion_exclusion_lists(expression) };
    }

    if (isConjunction(expression.child(0))) {
        std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> queries = getQueries(*expression.child(1));
        queries.push_back(get_inclusion_exclusion_lists(*expression.child(0)));
        return queries;
    }
    if (isConjunction(expression.child(1))) {
        std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> queries = getQueries(*expression.child(0));
        queries.push_back(get_inclusion_exclusion_lists(*expression.child(1)));
        return queries;
    }

    std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> lhs_queries = getQueries(*expression.child(0));
    std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> rhs_queries = getQueries(*expression.child(1));
    std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> queries;
    std::ranges::merge(lhs_queries, rhs_queries, std::back_inserter(queries));
    return queries;
}

QList<QPair<QList<QString>, QList<QString>>> getQueries(const QString& query_string)
{
    Expression expr;
    parseQuery(query_string.toStdString(), expr);
    const auto queries = getQueries(toDNF(expr));

    QList<QPair<QList<QString>, QList<QString>>> q;

    for (const auto& pair : queries) {
        const auto& [inclusion, exclusion] = pair;
        QList<QString> q_inclusion;
        QList<QString> q_exclusion;
        for (const auto& string : inclusion) {
            q_inclusion.push_back(QString::fromStdString(string));
        }
        for (const auto& string : exclusion) {
            q_exclusion.push_back(QString::fromStdString(string));
        }
        QPair<QList<QString>, QList<QString>> q_pair({q_inclusion, q_exclusion});
        q.push_back(q_pair);
    }

    return q;
}
