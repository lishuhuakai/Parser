#include "Common.h"
#include "Grammar.h"
#include "Rule.h"
#include "Symbol.h"
#include "FiniteStateAutomaton.h"
#include "ActionGoto.h"
/*
 * constructGrammar 用于构建一个文法.
 */
grammarPtr constructGrammar() {
	symbolPtr S = make_shared<Symbol>(Symbol::NonTerminal, L"S");
	symbolPtr E = make_shared<Symbol>(Symbol::NonTerminal, L"E");
	symbolPtr T = make_shared<Symbol>(Symbol::NonTerminal, L"T");
	symbolPtr n = make_shared<Symbol>(Symbol::Terminal, L"n");
	symbolPtr minus = make_shared<Symbol>(Symbol::Terminal, L"-");
	symbolPtr leftParathesis = make_shared<Symbol>(Symbol::Terminal, L"(");
	symbolPtr rightParathesis = make_shared<Symbol>(Symbol::Terminal, L")");
	rulePtr r1 = make_shared<Rule>(S);
	r1->rightHandAppend(E);
	rulePtr r2 = make_shared<Rule>(E);
	r2->rightHandAppend(E);
	r2->rightHandAppend(minus);
	r2->rightHandAppend(T);
	rulePtr r3 = make_shared<Rule>(E);
	r3->rightHandAppend(T);
	rulePtr r4 = make_shared<Rule>(T);
	r4->rightHandAppend(n);
	rulePtr r5 = make_shared<Rule>(T);
	r5->rightHandAppend(leftParathesis);
	r5->rightHandAppend(E);
	r5->rightHandAppend(rightParathesis);
	grammarPtr g = make_shared<Grammar>(S);
	g->appendRule(r1);
	g->appendRule(r2);
	g->appendRule(r3);
	g->appendRule(r4);
	g->appendRule(r5);
	return g;
}

int main() {
	auto g = constructGrammar();
	locale loc("chs");
	wcout.imbue(loc);
	FiniteStateAutomaton fa(*g);
	auto table = fa.constructDeterministicAutomaton();
	int n;
	wchar_t ch[10];
	while (true) {
		wstring c;
		wcin >> n;
		wcin >> ch;
		c = ch;
		table->queryAction(n, c);
		table->queryGoto(n, c);
	}
	getchar();
	return 0;
}