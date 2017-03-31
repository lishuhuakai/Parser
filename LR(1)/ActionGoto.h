#pragma once
#include "Common.h"
#include "Status.h"

/*
 * ActionGoto��Ҫ���ڼ�¼�������,���ƽ�,���ǹ�Լ.
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
	set<symbolPtr> symbols_;		/* ��¼���ս�� */
	map<int, itemSetPtr> mapping_;			/* ���ڼ�¼label��Item��ӳ�� */
	map<int, shared_ptr<map<symbolPtr, Action>>> action_;
	map<int, shared_ptr<map<symbolPtr, int>>> goto_;
	Grammar& g_;
private:
	void appendNewStat(int label,const shared_ptr<set<statusPtr>>& s);
	void recordRelations(int from, int to, symbolPtr& s);
	bool isReduceItem(itemSetPtr&);
};

