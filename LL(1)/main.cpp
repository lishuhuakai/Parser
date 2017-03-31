#include "Common.h"
#include "Grammar.h"
#include "Rule.h"
#include "Symbol.h"
#include "ParsingTable.h"

grammarPtr constructGrammar() { /* 构建规则 */
	symbolPtr session = make_shared<Symbol>(Symbol::NonTerminal, L"Session");
	symbolPtr facts = make_shared<Symbol>(Symbol::NonTerminal, L"Facts");
	symbolPtr question = make_shared<Symbol>(Symbol::NonTerminal, L"Question");
	symbolPtr leftBracket = make_shared<Symbol>(Symbol::Terminal, L"(");
	symbolPtr rightBracket = make_shared<Symbol>(Symbol::Terminal, L")");
	symbolPtr fact = make_shared<Symbol>(Symbol::NonTerminal, L"Fact");
	symbolPtr str = make_shared<Symbol>(Symbol::Terminal, L"STRING");
	symbolPtr exclaim = make_shared<Symbol>(Symbol::Terminal, L"!");
	symbolPtr ques = make_shared<Symbol>(Symbol::Terminal, L"?");
	rulePtr r1 = make_shared<Rule>(session);
	r1->rightHandAppend(facts);
	r1->rightHandAppend(question);
	rulePtr r2 = make_shared<Rule>(session);
	r2->rightHandAppend(leftBracket);
	r2->rightHandAppend(session);
	r2->rightHandAppend(rightBracket);
	r2->rightHandAppend(session);
	rulePtr r3 = make_shared<Rule>(facts);
	r3->rightHandAppend(fact);
	r3->rightHandAppend(facts);
	rulePtr r4 = make_shared<Rule>(facts);
	r4->rightHandAppend(Symbol::eps);
	rulePtr r5 = make_shared<Rule>(fact);
	r5->rightHandAppend(exclaim);
	r5->rightHandAppend(str);
	rulePtr r6 = make_shared<Rule>(question);
	r6->rightHandAppend(ques);
	r6->rightHandAppend(str);

	grammarPtr g = make_shared<Grammar>(session);
	g->appendRule(r1);
	g->appendRule(r2);
	g->appendRule(r3);
	g->appendRule(r4);
	g->appendRule(r5);
	g->appendRule(r6);
	return g;
}

int main() {
	auto g = constructGrammar();
	//wcout << *g;
	//g->calcFirstSet();
	ParsingTable table(*g);
	table.buildParsingTable();
	//g->printFirstSet();
	//g->confirmCouldProductEmptyStr();
	//g->calcFollowSet();
	getchar();
	return 0;
}