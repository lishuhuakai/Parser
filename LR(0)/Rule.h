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
	bool isEpsRule() {							/* �����ж����������Ƿ��ǦŹ��� */
		return (right_.size() == 1) && (right_[0]->isEps());
	}
	bool leftHandEqualTo(symbolPtr& s) {		/* �ж�rule������Ƿ���s */
		return s == left_;
	}
	symbolPtr findNthElem(size_t n) {			/* Ѱ��rule�Ƶ�ʽ�Ҳ�ĵ�N��Ԫ�� */
		assert(n < right_.size());
		return right_[n];
	}
	size_t rightHandLength() {
		return right_.size();
	}
	symbolPtr leftHandSymbol() {				/* ��ȡ�������ķ��ս�� */
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
	symbolPtr left_;					/* ��߲��� */
	vector<symbolPtr> right_;			/* �ұ߲��� */
	set<symbolPtr> terminals_;		/* ���ڼ�¼�ս�� */
};

