#include "ActionGoto.h"



ActionGoto::ActionGoto()
{
}


ActionGoto::~ActionGoto()
{
}

/*
 * printTable 用于输出表格的信息.
 */
void ActionGoto::printTable()
{
	wcout << L"status:" << endl;
	for (auto pr : stats_) {
		wcout << "label: " << pr.first << endl;
		wcout << pr.second << endl;
	}

	wcout << L"relations:" << endl;
	for (auto line : table_) {
		int from = line.first;
		for (auto row : *(line.second)) {
			symbolPtr s = row.first;
			wcout << L"[" << from << L"," << *s << "] " << row.second << endl;
		}
		wcout << endl;
	}
}

/*
 * appendNewStat 用于添加新的状态.
 */
void ActionGoto::appendNewStat(int label, const shared_ptr<set<statusPtr>>& s)
{
	shared_ptr<set<Item>> itemSet = make_shared<set<Item>>();
	for (auto stat : *s) {
		itemSet->insert(stat->items.begin(), stat->items.end());
	}
	stats_[label] = Record(itemSet);
	table_[label] = make_shared<map<symbolPtr, Action>>(); /* 构建新item */
}

/*
 * recordRelations用于记录状态和状态之间的关系.
 */
void ActionGoto::recordRelations(int from, int to, symbolPtr & s)
{
	auto mp = table_[from];
	(*mp)[s] = Action(Action::shift, to);
	stats_[from].type = Record::shift; /* from方一定是移进项 */
	symbols_.insert(s);
}

/*
 * buildTable 用于完整地构建ActionGoto表.
 */
void ActionGoto::buildTable()
{
	for (auto record : stats_) {
		int from = record.first;
		Action a(Action::reduce, from);
		if (record.second.type == Record::reduce) {
			for (auto nt : symbols_) {
				(*table_[from])[nt] = a;	/* 按照from状态的规则来规约 */
			}
		}
	}
}
