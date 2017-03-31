#include "ActionGoto.h"
#include "Grammar.h"

ActionGoto::ActionGoto(Grammar& g) :
	g_(g)
{
}


ActionGoto::~ActionGoto()
{
}

void ActionGoto::queryAction(int pos, wstring& str)
{
	symbolPtr s = g_.str2symbolPtr(str);
	auto act = (*action_[pos])[s];
	wcout << act << endl;
}

void ActionGoto::queryGoto(int pos, wstring& str)
{
	symbolPtr s = g_.str2symbolPtr(str);
	wcout << (*goto_[pos])[s] << endl;
}

/*
 * appendNewStat ��������µ�״̬.
 */
void ActionGoto::appendNewStat(int label, const shared_ptr<set<statusPtr>>& s)
{
	itemSetPtr items = make_shared<itemSet>();
	for (auto stat : *s) {
		items->insert(stat->items.begin(), stat->items.end());
	}
	/* ����һ���µ�ӳ�� */
	mapping_[label] = items;
	action_[label] = make_shared<map<symbolPtr, Action>>();
	goto_[label] = make_shared<map<symbolPtr, int>>();
	/* ����ǹ�Լ��,��ôҪ��ʼ��¼ */
	if (isReduceItem(items)) {
		for (auto it : *items) {
			(*action_[label])[it.getLookAhead()] = Action(it.getRule());
		}
	}
}

/*
 * recordRelations���ڼ�¼״̬��״̬֮��Ĺ�ϵ.
 */
void ActionGoto::recordRelations(int from, int to, symbolPtr & s)
{
	/* ������Ҫ���ڼ�¼�ƽ��� */
	(*goto_[from])[s] = to;  /* goto���¼��ת������ */
	if (s->isTerminal())
		(*action_[from])[s] = Action();

}

/*
 * isReduceItem �ж��Ƿ��ǹ�Լ��
 */
bool ActionGoto::isReduceItem(itemSetPtr& its)
{
	/* ��Լ����ص���,ÿһ��item��pos���ﵽ���յ�,���Խ��й�Լ��. */
	for (auto it : *its) {
		if (it.getPos() != it.getRule()->rightHandLength())
			return false;
	}
	return true;
}

