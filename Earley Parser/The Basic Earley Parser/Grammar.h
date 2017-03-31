#pragma once
#include "Common.h"
class Rule;
class Symbol;

class Grammar
{
public:
	Grammar(symbolPtr&);
	~Grammar();
public:
	vector<rulePtr> rules_;  /* 用于记录规则 */
	symbolPtr startSymbol_; /* 开始符号 */
public:
	shared_ptr<vector<rulePtr>> findRules(symbolPtr& l);  /* 通过l,寻找以l为开头的rule */
	friend wostream& operator<<(wostream& os, Grammar& g);
};

