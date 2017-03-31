#include "Grammar.h"
#include "Rule.h"
#include "Symbol.h"


Grammar::Grammar(symbolPtr& s) :
	startSymbol_(s)
{
}


Grammar::~Grammar()
{
}

/*
 * findRules 用于寻找非终结符l的推导规则.
 */
shared_ptr<vector<rulePtr>> Grammar::findRules(symbolPtr & l)
{
	assert(l->type_ == Symbol::NonTerminal);
	shared_ptr<vector<rulePtr>> result = make_shared<vector<rulePtr>>();
	for (auto rule : rules_) {
		if (rule->startWith(l)) {
			result->push_back(rule);
		}
	}
	return result;
}

wostream & operator<<(wostream & os, Grammar & g)
{
	for (auto rule : g.rules_) {
		os << *rule;
	}
	return os;
}
