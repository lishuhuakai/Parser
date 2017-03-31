#include "ActionGoto.h"



ActionGoto::ActionGoto()
{
}


ActionGoto::~ActionGoto()
{
}

/*
 * printTable �������������Ϣ.
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
 * appendNewStat ��������µ�״̬.
 */
void ActionGoto::appendNewStat(int label, const shared_ptr<set<statusPtr>>& s)
{
	shared_ptr<set<Item>> itemSet = make_shared<set<Item>>();
	for (auto stat : *s) {
		itemSet->insert(stat->items.begin(), stat->items.end());
	}
	stats_[label] = Record(itemSet);
	table_[label] = make_shared<map<symbolPtr, Action>>(); /* ������item */
}

/*
 * recordRelations���ڼ�¼״̬��״̬֮��Ĺ�ϵ.
 */
void ActionGoto::recordRelations(int from, int to, symbolPtr & s)
{
	auto mp = table_[from];
	(*mp)[s] = Action(Action::shift, to);
	stats_[from].type = Record::shift; /* from��һ�����ƽ��� */
	symbols_.insert(s);
}

/*
 * buildTable ���������ع���ActionGoto��.
 */
void ActionGoto::buildTable()
{
	for (auto record : stats_) {
		int from = record.first;
		Action a(Action::reduce, from);
		if (record.second.type == Record::reduce) {
			for (auto nt : symbols_) {
				(*table_[from])[nt] = a;	/* ����from״̬�Ĺ�������Լ */
			}
		}
	}
}
