#pragma once
class Grammar;
#include "Status.h"
#include "Rule.h"

/*
 * LALRParser和之前的LR(1)有所不同,因为它有更加简洁高效的实现方法.
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
	/* 使用vector来存储移进时使用的规则 */
	vector<rulePtr> reducePool_;
	map<size_t, map<symbolPtr, Action>> table_;
	size_t start_;		/* 用于记录开始状态的下标 */
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

