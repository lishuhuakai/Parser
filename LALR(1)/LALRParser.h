#pragma once
class Grammar;
#include "Status.h"
#include "Rule.h"

/*
 * LALRParser��֮ǰ��LR(1)������ͬ,��Ϊ���и��Ӽ���Ч��ʵ�ַ���.
 */
class LALRParser
{
public:
	LALRParser(Grammar&);
	~LALRParser();
private:
	struct Action {
		enum action {shift, reduce};
		action act;
		int index;
		Action(action act, int index) :
			act(act), index(index)
		{}
		Action() {}
	};

	Grammar& g_;
	map<size_t, statusPtr> status_;
	/* ʹ��vector���洢�ƽ�ʱʹ�õĹ��� */
	vector<rulePtr> reducePool_;
	map<size_t, map<symbolPtr, Action>> table_;
	size_t start_;		/* ���ڼ�¼��ʼ״̬���±� */
public:
	void printStats();
	void computerLookAhead(); 
private:
	Action makeAction(Action::action, size_t);
	Action makeAction(Action::action, rulePtr&);
	static size_t appendAction(map<symbolPtr, shared_ptr<list<Action>>>&, symbolPtr&, Action&);
	size_t expandRule(list<Item>&, bool&);
	shared_ptr<map<symbolPtr, shared_ptr<list<Item>>>> classify(statusPtr& s, list<Item>&);
};

