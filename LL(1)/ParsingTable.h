#pragma once
#include "Common.h"
/*
 * ParsingTable 用于构建一张解析表,这张表很有意思,横轴表示非终结符,横轴表示终结符.
 * ParsingTable	感觉要用两个嵌套的map来实现.
 */
class ParsingTable
{
public:
	ParsingTable(Grammar& g);
	~ParsingTable();
	map<symbolPtr, rulePtr>& operator[](symbolPtr& p);
	void buildParsingTable();
private:
	map<symbolPtr, shared_ptr<map<symbolPtr, rulePtr>>> table_;
	Grammar& grammar_;
private:
	void calcFirstSet(rulePtr& rule, set<symbolPtr>& firstSet);
};

