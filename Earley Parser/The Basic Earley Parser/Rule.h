#pragma once
#include "Common.h"
#include "Symbol.h"
/*
 * Rule����Ҫ���ڼ�¼�﷨,����˵�ķ�.
 */
class Rule
{
public:
	Rule(symbolPtr&);
	~Rule();
public:
	bool startWith(symbolPtr& s) { /* �ж�rule������Ƿ���s */
		return s == left_;
	}
	symbolPtr findNthElem(int n) { /* Ѱ��rule�Ƶ�ʽ�Ҳ�ĵ�N��Ԫ�� */
		assert(n < right_.size());
		return right_[n];
	}
	size_t length() {
		return right_.size();
	}
	symbolPtr startSymbol() { /* ��ȡ��ʼ���� */
		return left_;
	}
	Rule(const Rule& r);
public:
	friend wostream& operator<<(wostream& os, Rule& r);
public:
	symbolPtr left_;		  /* ��߲��� */
	vector<symbolPtr> right_; /* �ұ߲��� */
};

