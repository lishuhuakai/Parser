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
 * appendNewStat 用于添加新的状态.
 */
void ActionGoto::appendNewStat(int label, const shared_ptr<set<statusPtr>>& s)
{
	itemSetPtr items = make_shared<itemSet>();
	for (auto stat : *s) {
		items->insert(stat->items.begin(), stat->items.end());
	}
	/* 构建一条新的映射 */
	mapping_[label] = items;
	action_[label] = make_shared<map<symbolPtr, Action>>();
	goto_[label] = make_shared<map<symbolPtr, int>>();
	/* 如果是归约项,那么要开始记录 */
	if (isReduceItem(items)) {
		for (auto it : *items) {
			(*action_[label])[it.getLookAhead()] = Action(it.getRule());
		}
	}
}

/*
 * recordRelations用于记录状态和状态之间的关系.
 */
void ActionGoto::recordRelations(int from, int to, symbolPtr & s)
{
	/* 这里主要用于记录移进项 */
	(*goto_[from])[s] = to;  /* goto表记录下转换规则 */
	if (s->isTerminal())
		(*action_[from])[s] = Action();

}

/*
 * isReduceItem 判断是否是归约项
 */
bool ActionGoto::isReduceItem(itemSetPtr& its)
{
	/* 归约项的特点是,每一个item的pos都达到了终点,可以进行规约啦. */
	for (auto it : *its) {
		if (it.getPos() != it.getRule()->rightHandLength())
			return false;
	}
	return true;
}

