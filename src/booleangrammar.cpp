#include "booleangrammar.hpp"

#include <boost/phoenix.hpp>

std::string ASTFormatter::operator()(const std::string& expr) const
{
    return expr;
}

std::string ASTFormatter::operator()(const UnaryNode& expr) const
{
    return "!" + boost::apply_visitor(ASTFormatter(), expr.scope);
}

std::string ASTFormatter::operator()(const BinaryNode& expr) const
{
    return "(" + boost::apply_visitor(ASTFormatter(), expr.lhs)
               + " "
               + expr.op
               + " "
               + boost::apply_visitor(ASTFormatter(), expr.rhs)
               + ")";
}

BooleanGrammar::BooleanGrammar() : base_type(start)
{
    using namespace qi::labels;
    using qi::skip;
    using ascii::char_;
    using ascii::alnum;
    using ascii::space;

    const auto op = [](auto... sym) { return qi::copy((qi::string(sym) | ...)); };

    start = skip(space)[qi::eps >> disjunction] >> qi::eoi;

    disjunction %= conjunction >> *(op("|") >> conjunction)[_val = boost::phoenix::construct<BinaryNode>(_1, _val, _2)];
    conjunction %= negation >> *(op("&") >> negation)[_val = boost::phoenix::construct<BinaryNode>(_1, _val, _2)];

    negation = term[_val = _1]
               | (op("!") >> negation)[_val = boost::phoenix::construct<UnaryNode>(_1, _2)]
               | ("(" >> disjunction[_val = _1] >> ")");

    term = +alnum
           | qi::lexeme[char_('"') >> +~char_('"') >> char_('"')];
}

bool parse(const std::string& query, AST& parsetree)
{
    auto begin = query.cbegin();
    auto end = query.cend();
    bool const result = boost::spirit::qi::parse(begin, end, BooleanGrammar(), parsetree);
    if (begin != end) {
        return false;
    }
    return true;
}

bool checkQuery(const std::string& query)
{
    auto begin = query.cbegin();
    auto end = query.cend();
    bool const result = boost::spirit::qi::parse(begin, end, BooleanGrammar());
    if (begin != end) {
        return false;
    }
    return true;
}
