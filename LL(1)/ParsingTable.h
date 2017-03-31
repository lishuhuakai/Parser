#pragma once
#include "Common.h"
/*
 * ParsingTable ���ڹ���һ�Ž�����,���ű������˼,�����ʾ���ս��,�����ʾ�ս��.
 * ParsingTable	�о�Ҫ������Ƕ�׵�map��ʵ��.
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

