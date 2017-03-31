#include "Dfa.h"
#include "Status.h"
#include "Nfa.h"
#include "ActionGoto.h"

typedef shared_ptr<set<statusPtr>> statusSetPtr;

struct NewStatus {				/* 我们需要构建一个新的数据结构 */
	statusSetPtr oldStatusSet;	/* 用于记录所有的状态的集合 */
	int label;					/* 每一个新状态都有一个标签 */
	NewStatus(statusSetPtr& ptr, int lb) :
		oldStatusSet(ptr),
		label(lb)
	{ }
};

typedef shared_ptr<NewStatus>  newStatusPtr;

struct Compare {
	bool operator()(const newStatusPtr& l1, const newStatusPtr& l2) const
	{
		return *((*l1).oldStatusSet) > *((*l2).oldStatusSet); /* 直接比较两个指针比较的内容,而不是指针的值 */
	}
};


Dfa::Dfa(Nfa& n) :
	nfa_(n),
	builted(false)
{
}


Dfa::~Dfa()
{
}


shared_ptr<ActionGoto> Dfa::buildDFA() {
	shared_ptr<ActionGoto> table = make_shared<ActionGoto>();
	statusPtr start = nfa_.buildNfa();
	queue<newStatusPtr> l;			/* 队列l */
	set<newStatusPtr, Compare> d;	/* 集合d */
	/* 首先将起始状态放入l和d中 */
	statusSetPtr oldStatusSet = make_shared<set<statusPtr>>();
	oldStatusSet->insert(start);	/* 从起始状态出发 */
	table->appendNewStat(0, oldStatusSet); /* 添加起始状态 */
	newStatusPtr startPtr = make_shared<NewStatus>(oldStatusSet, assignNewStatusLabel());
	d.insert(startPtr);
	l.push(startPtr);

	while (!l.empty()) {
		/* 从队列中取出一个状态 */
		newStatusPtr s = l.front(); l.pop();
		/* 计算从这个状态出发,所接收的字符的并集 */
		set<symbolPtr> symbols;
		getAllSymbols(s->oldStatusSet, symbols);
		
		for (auto sm : symbols) {	/* 对于每一个字符 */
			statusSetPtr oldStatusSet = make_shared<set<statusPtr>>();
			bool finalTransfer = false;
			for (auto oldStatus : *s->oldStatusSet) {			/* 对于组成新节点的每个旧节点 */
				for (auto edge : oldStatus->outEdges) {			/* 遍历每一条出边 */
					if (findElement(edge->matchContent, sm)) {	/* 如果能够匹配的话 */
						oldStatusSet->insert(edge->to);
					}
				}
			}
			/* 运行到这里需要注意的一点是,oldStatusSet不可能为空 */
			assert(oldStatusSet->size() != 0);
			newStatusPtr newStatus = make_shared<NewStatus>(oldStatusSet, 0);
			auto pos = d.find(newStatus);
			int toLabel = 0;
			if (pos != d.end()) { /* 这个状态已经在集合D中存在了 */
				toLabel = (*pos)->label;
			}
			else { /* 如果不存在的话,需要重新非配一个label */
				toLabel = assignNewStatusLabel();
				table->appendNewStat(toLabel, oldStatusSet);
				newStatus->label = toLabel;
				d.insert(newStatus);
				l.push(newStatus);
			}
			table->recordRelations(s->label, toLabel, sm);	/* 添加关系 */
		}
	}
	nfa_.clearEnv();
	table->buildTable();
	return table;
}


void Dfa::getAllSymbols(shared_ptr<set<statusPtr>>& st, set<symbolPtr>& res) {
	/* o(n^3) 效率实在不怎么样,但是优化的地步也不大 */
	for (auto status : (*st)) {						/* 对于每一个状态 */
		for (auto edge : (*status).outEdges) {		/* 对于每一条出边 */
			for (auto i : edge->matchContent)
				res.insert(i);
		}
	}
}

/*
 * assignNewStatusLabel 用于分配一个新的编号
 */
int Dfa::assignNewStatusLabel() { 
	static int newStatusCount = 0;
	return newStatusCount++;
}