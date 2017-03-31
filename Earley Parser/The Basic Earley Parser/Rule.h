#pragma once
#include "Common.h"
#include "Symbol.h"
/*
 * Rule类主要用于记录语法,或者说文法.
 */
class Rule
{
public:
	Rule(symbolPtr&);
	~Rule();
public:
	bool startWith(symbolPtr& s) { /* 判断rule的左边是否是s */
		return s == left_;
	}
	symbolPtr findNthElem(int n) { /* 寻找rule推导式右侧的第N个元素 */
		assert(n < right_.size());
		return right_[n];
	}
	size_t length() {
		return right_.size();
	}
	symbolPtr startSymbol() { /* 获取开始符号 */
		return left_;
	}
	Rule(const Rule& r);
public:
	friend wostream& operator<<(wostream& os, Rule& r);
public:
	symbolPtr left_;		  /* 左边部分 */
	vector<symbolPtr> right_; /* 右边部分 */
};

