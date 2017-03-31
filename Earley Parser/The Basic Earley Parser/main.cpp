#include "Common.h"
#include "Rule.h"
#include "Grammar.h"
#include "Symbol.h"
#include "EarleyParser.h"

/*
 * createGrammar用与于构建一个测试用的文法.
 */
grammarPtr createGrammar() {
	/* 在这里,我来构建一波非常简单的语法 */
	symbolPtr start = make_shared<Symbol>(Symbol::NonTerminal, L"S");
	symbolPtr E = make_shared<Symbol>(Symbol::NonTerminal, L"E");
	symbolPtr Q = make_shared<Symbol>(Symbol::NonTerminal, L"Q");
	symbolPtr F = make_shared<Symbol>(Symbol::NonTerminal, L"F");
	symbolPtr a = make_shared<Symbol>(Symbol::Terminal, L"a");
	symbolPtr plus = make_shared<Symbol>(Symbol::Terminal, L"+");
	symbolPtr minus = make_shared<Symbol>(Symbol::Terminal, L"-");
	rulePtr r1 = make_shared<Rule>(start);
	r1->right_.push_back(E);
	rulePtr r2 = make_shared<Rule>(E);
	r2->right_.push_back(E);
	r2->right_.push_back(Q);
	r2->right_.push_back(F);
	rulePtr r3 = make_shared<Rule>(E);
	r3->right_.push_back(F);
	rulePtr r4 = make_shared<Rule>(F);
	r4->right_.push_back(a);
	rulePtr r5 = make_shared<Rule>(Q);
	r5->right_.push_back(plus);
	rulePtr r6 = make_shared<Rule>(Q);
	r6->right_.push_back(minus);
	grammarPtr g = make_shared<Grammar>(start);
	g->rules_.push_back(r1);
	g->rules_.push_back(r2);
	g->rules_.push_back(r3);
	g->rules_.push_back(r4);
	g->rules_.push_back(r5);
	g->rules_.push_back(r6);
	return g;
}

int main() {
	auto g = createGrammar();
	//wcout << *g;
	locale loc("chs");
	wcout.imbue(loc);
	EarleyParser p(g);
	vector<symbolPtr> str;
	str.push_back(make_shared<Symbol>(Symbol::Terminal, L"a"));
	str.push_back(make_shared<Symbol>(Symbol::Terminal, L"-"));
	str.push_back(make_shared<Symbol>(Symbol::Terminal, L"a"));
	str.push_back(make_shared<Symbol>(Symbol::Terminal, L"+"));
	str.push_back(make_shared<Symbol>(Symbol::Terminal, L"a"));
	bool success = p.startParse(str);
	getchar();
	return 0;
}