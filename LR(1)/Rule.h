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
	bool isEpsRule() {							/* 用于判断这条规则是否是ε规则 */
		return (right_.size() == 1) && (right_[0]->isEps());
	}
	bool leftHandEqualTo(symbolPtr& s) {		/* 判断rule的左边是否是s */
		return s == left_;
	}
	symbolPtr findNthElem(size_t n) {			/* 寻找rule推导式右侧的第N个元素 */
		assert(n < right_.size());
		return right_[n];
	}
	size_t rightHandLength() {
		return right_.size();
	}
	symbolPtr leftHandSymbol() {				/* 获取规则左侧的非终结符 */
		return left_;
	}
	Rule(const Rule& r);
	void rightHandAppend(symbolPtr& s) {
		if (s->isTerminal() && !s->isEps()) {
			terminals_.insert(s);
		}
		right_.push_back(s);
	}
	const set<symbolPtr>& getTerminals() const {
		return terminals_;
	}
public:
	friend wostream& operator<<(wostream& os, Rule& r);
	bool operator==(Rule& r);
private:
	symbolPtr left_;					/* 左边部分 */
	vector<symbolPtr> right_;			/* 右边部分 */
	set<symbolPtr> terminals_;		/* 用于记录终结符 */
};

