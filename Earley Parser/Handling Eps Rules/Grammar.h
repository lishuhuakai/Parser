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
	vector<rulePtr> rules_;  /* ���ڼ�¼���� */
	symbolPtr startSymbol_; /* ��ʼ���� */
public:
	shared_ptr<vector<rulePtr>> findRules(symbolPtr& l);  /* ͨ��l,Ѱ����lΪ��ͷ��rule */
	friend wostream& operator<<(wostream& os, Grammar& g);
};

