#include "Common.h"
#include "Grammar.h"
#include "Rule.h"
#include "Symbol.h"
#include "LALRParser.h"
#include <regex>
/*
 * constructGrammar ���ڹ���һ���ķ�.
 */
grammarPtr constructGrammar() {
	symbolPtr start = make_shared<Symbol>(Symbol::NonTerminal, L"start");
	symbolPtr S = make_shared<Symbol>(Symbol::NonTerminal, L"S");
	symbolPtr E = make_shared<Symbol>(Symbol::NonTerminal, L"E");
	symbolPtr end = make_shared<Symbol>(Symbol::Terminal, L"#");
	symbolPtr T = make_shared<Symbol>(Symbol::NonTerminal, L"T");
	symbolPtr n = make_shared<Symbol>(Symbol::Terminal, L"n");
	symbolPtr minus = make_shared<Symbol>(Symbol::Terminal, L"-");
	symbolPtr leftParathesis = make_shared<Symbol>(Symbol::Terminal, L"(");
	symbolPtr rightParathesis = make_shared<Symbol>(Symbol::Terminal, L")");
	rulePtr r0 = make_shared<Rule>(start);
	r0->expansionAppend(S);
	r0->expansionAppend(end);
	rulePtr r1 = make_shared<Rule>(S);
	r1->expansionAppend(E);
	//r1->expansionAppend(end);
	rulePtr r2 = make_shared<Rule>(E);
	r2->expansionAppend(E);
	r2->expansionAppend(minus);
	r2->expansionAppend(T);
	rulePtr r3 = make_shared<Rule>(E);
	r3->expansionAppend(T);
	rulePtr r4 = make_shared<Rule>(T);
	r4->expansionAppend(n);
	rulePtr r5 = make_shared<Rule>(T);
	r5->expansionAppend(leftParathesis);
	r5->expansionAppend(E);
	r5->expansionAppend(rightParathesis);
	grammarPtr g = make_shared<Grammar>(start);
	g->appendRule(r0);
	g->appendRule(r1);
	g->appendRule(r2);
	g->appendRule(r3);
	g->appendRule(r4);
	g->appendRule(r5);
	return g;
}

grammarPtr constructGrammar1() { /* �������� */
	symbolPtr start = make_shared<Symbol>(Symbol::NonTerminal, L"start");
	symbolPtr session = make_shared<Symbol>(Symbol::NonTerminal, L"Session");
	symbolPtr facts = make_shared<Symbol>(Symbol::NonTerminal, L"Facts");
	symbolPtr question = make_shared<Symbol>(Symbol::NonTerminal, L"Question");
	symbolPtr leftBracket = make_shared<Symbol>(Symbol::Terminal, L"(");
	symbolPtr rightBracket = make_shared<Symbol>(Symbol::Terminal, L")");
	symbolPtr fact = make_shared<Symbol>(Symbol::NonTerminal, L"Fact");
	symbolPtr str = make_shared<Symbol>(Symbol::Terminal, L"STRING");
	symbolPtr exclaim = make_shared<Symbol>(Symbol::Terminal, L"!");
	symbolPtr ques = make_shared<Symbol>(Symbol::Terminal, L"?");
	symbolPtr end = make_shared<Symbol>(Symbol::Terminal, L"#");
	rulePtr r0 = make_shared<Rule>(start);
	r0->expansionAppend(session);
	r0->expansionAppend(end);
	rulePtr r1 = make_shared<Rule>(session);
	r1->expansionAppend(facts);
	r1->expansionAppend(question);
	//r1->expansionAppend(end);
	rulePtr r2 = make_shared<Rule>(session);
	r2->expansionAppend(leftBracket);
	r2->expansionAppend(session);
	r2->expansionAppend(rightBracket);
	r2->expansionAppend(session);
	//r2->expansionAppend(end);
	rulePtr r3 = make_shared<Rule>(facts);
	r3->expansionAppend(fact);
	r3->expansionAppend(facts);
	rulePtr r4 = make_shared<Rule>(facts);
	//r4->expansionAppend(Symbol::eps);
	rulePtr r5 = make_shared<Rule>(fact);
	r5->expansionAppend(exclaim);
	r5->expansionAppend(str);
	rulePtr r6 = make_shared<Rule>(question);
	r6->expansionAppend(ques);
	r6->expansionAppend(str);

	grammarPtr g = make_shared<Grammar>(start);
	g->appendRule(r0);
	g->appendRule(r1);
	g->appendRule(r2);
	g->appendRule(r3);
	g->appendRule(r4);
	g->appendRule(r5);
	g->appendRule(r6);
	return g;
}

void demo() {

}

int main() {
	auto g = constructGrammar();
	//size_t h1 = std::hash<shared_ptr<Grammar>>{}(g);
	//auto k = g;
	//size_t h2 = std::hash<shared_ptr<Grammar>>{}(k);
	//wcout << (h1 == h2) << endl;
	//wcout << *g;
	locale loc("chs");
	wcout.imbue(loc);
	//g->calculateSets();
	//g->printSets();
	LALRParser lalr = LALRParser(*g);
	lalr.computerLookAhead();
	//lalr.printStats();
	getchar();
	return 0;
}