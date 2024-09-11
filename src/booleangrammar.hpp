#pragma once

#include <string>
#include <utility>

#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>

struct UnaryNode;
struct BinaryNode;

using expression = boost::variant<std::string, boost::recursive_wrapper<UnaryNode>, boost::recursive_wrapper<BinaryNode>>;

struct AST : expression {
    using expression::expression;
    using expression::operator=;
};

struct UnaryNode {
    UnaryNode(std::string op, AST scope)
        : op(std::move(op))
        , scope(std::move(scope))
    {}

    std::string op;
    AST scope;
};

struct BinaryNode {
    BinaryNode(std::string op, AST lhs, AST rhs)
        : op(std::move(op))
        , lhs(std::move(lhs))
        , rhs(std::move(rhs))
    {}

    std::string op;
    AST lhs;
    AST rhs;
};

struct ASTFormatter : public boost::static_visitor<std::string> {
    std::string operator()(const std::string& expr) const;
    std::string operator()(const UnaryNode& expr) const;
    std::string operator()(const BinaryNode& expr) const;
};

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

class BooleanGrammar : public qi::grammar<std::string::const_iterator, AST()> {
public:
    BooleanGrammar();

private:
    qi::rule<std::string::const_iterator, AST()> start;
    qi::rule<std::string::const_iterator, AST(), ascii::space_type> disjunction, conjunction, negation;
    qi::rule<std::string::const_iterator, std::string()> term;
};

bool parse(const std::string& query, AST& parsetree);
bool checkQuery(const std::string& query);
