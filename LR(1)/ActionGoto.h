#pragma once
#include "Common.h"
#include "Status.h"

/*
 * ActionGoto主要用于记录如何行走,是移进,还是规约.
 */
class ActionGoto
{
	friend class FiniteStateAutomaton;
public:
	ActionGoto(Grammar&);
public:
	~ActionGoto();
public:
	struct Action {
		enum Type { kShift, kReduce};
		Type type;
		rulePtr rule;
		Action() :
			type(kShift)
		{}
		Action(rulePtr& r) :
			type(kReduce), rule(r)
		{}
		friend wostream& operator<<(wostream& os, Action& act) {
			switch (act.type)
			{
			case kShift:
				os << L"s";
				break;
			case kReduce:
				os << L"r " << *act.rule;
				break;
			default:
				break;
			}
			return os;
		}
	};
public:
	void queryAction(int pos, wstring&);
	void queryGoto(int pos, wstring&);
private:
	typedef set<Item> itemSet;
	typedef shared_ptr<set<Item>> itemSetPtr;
	set<symbolPtr> symbols_;		/* 记录非终结符 */
	map<int, itemSetPtr> mapping_;			/* 用于记录label到Item的映射 */
	map<int, shared_ptr<map<symbolPtr, Action>>> action_;
	map<int, shared_ptr<map<symbolPtr, int>>> goto_;
	Grammar& g_;
private:
	void appendNewStat(int label,const shared_ptr<set<statusPtr>>& s);
	void recordRelations(int from, int to, symbolPtr& s);
	bool isReduceItem(itemSetPtr&);
};

