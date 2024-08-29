#pragma once

#include "expression.hpp"

bool isNegation(std::shared_ptr<Expression> expr);
bool isNegation(const Expression& expr);
bool isDisjunction(std::shared_ptr < Expression> expr);
bool isDisjunction(const Expression& expr);
bool isConjunction(std::shared_ptr<Expression> expr);
bool isConjunction(const Expression& expr);
bool isAtomic(std::shared_ptr<Expression> expr);
bool isAtomic(const Expression& expr);

bool isNNF(const Expression& expr);
bool isDNF(const Expression& expr);

Expression toDNF(const Expression& expr);
Expression toNNF(const Expression& expr);

Expression NNFtoDNF(const Expression& expr);
