#pragma once
#include "Common.h"
#include "Status.h"

/*
 * ActionGoto主要用于记录如何行走,是移进,还是规约.
 */
class ActionGoto
{
	friend class Dfa;
public:
	ActionGoto();
	~ActionGoto();
public:
	void printTable();
private:
	struct Action {
		enum Type { shift, reduce, error};
		Type type;
		int status;		/* 用于记录到达哪一个状态 */
		Action() :
			type(error), status(-1)
		{}
		Action(Type t, int s) :
			type(t), status(s)
		{}
		friend wostream& operator<<(wostream& os, Action& it) {
			switch (it.type)
			{
			case shift:
				os << L"s";
				break;
			case reduce:
				os << L"r";
				break;
			case error:
				os << L"e";
				break;
			default:
				break;
			}
			os << it.status << endl;
			return os;
		}
	};

	struct Record {
		enum Type {shift, reduce};
		Type type;
		shared_ptr<set<Item>> items;

		Record(const shared_ptr<set<Item>>& it) :
			items(it), type(reduce)
		{}
		Record(const Record& r) {
			type = r.type;
			items = r.items;
		}
		Record():
			type(reduce)
		{}
		friend wostream& operator<<(wostream& os, Record& r) {
			if (r.type == shift)
				os << "shift Item" << endl;
			else
				os << "reduce Item" << endl;
			for (auto it : *(r.items)) {
				os << it << endl;
			}
			return os;
		}
	};
private:
	void appendNewStat(int label,const shared_ptr<set<statusPtr>>& s);
	void recordRelations(int from, int to, symbolPtr& s);
	void buildTable();
private:
	map<int, Record> stats_;
	set<symbolPtr> symbols_;		/* 记录非终结符 */
	map<int, shared_ptr<map<symbolPtr, Action>>> table_;
};

