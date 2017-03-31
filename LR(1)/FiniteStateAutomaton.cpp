#include "FiniteStateAutomaton.h"
#include "ActionGoto.h"
#include <functional>
using namespace std;


FiniteStateAutomaton::FiniteStateAutomaton(Grammar& g) :
	g_(g)
{
}

FiniteStateAutomaton::~FiniteStateAutomaton()
{
}

statusPtr FiniteStateAutomaton::makeNewStat(Item& it) {
	int key = nextLabel();
	statusPtr s(new Status(key, it), std::bind(&FiniteStateAutomaton::deleteStatusPtr, this, placeholders::_1));
	allStatus_[key] = s;
	return s;
}

statusPtr FiniteStateAutomaton::makeNewStation(symbolPtr& s1, symbolPtr& s2)
{
	int key = nextLabel();
	statusPtr s(new Status(key, s1, s2), std::bind(&FiniteStateAutomaton::deleteStatusPtr, this, placeholders::_1));
	allStatus_[key] = s;
	return s;
}

edgePtr FiniteStateAutomaton::makeNewEdge(const statusPtr& from, const statusPtr& to) {
	int key = nextLabel2();
	edgePtr e(new Edge(key, from, to), bind(&FiniteStateAutomaton::deleteEdgePtr, this, placeholders::_1));
	allEdges_[key] = e;
	return e;
}


/*
 * constructNonDeterministicAutomaton 为LR(1)来构建非确定性自动机.
 */
statusPtr FiniteStateAutomaton::constructNonDeterministicAutomaton()
{
	map<wstring, statusPtr> stationsMap; /* 用于记录所有的station */
	queue<statusPtr> stations;
	symbolPtr first = g_.startSymbol_;
	symbolPtr second = Symbol::eof;
	statusPtr head = makeNewStation(first, second);
	stationsMap[first->uniqueMark() + second->uniqueMark()] = head; /* 放入到stationsMap中 */
	stations.push(head);

	while (!stations.empty()) {
		auto station = stations.front(); stations.pop();  /* 取出一个station */
		//wcout << *station << endl;
		subsetAlgorithm(station, stationsMap, stations);
	}
	//printGraph();
	return head;
}


/*
 * status2Station 用于连接status和statioon.
 */
 void FiniteStateAutomaton::status2Station(statusPtr& s, symbolPtr& lookAhead, map<wstring, statusPtr>& sm, queue<statusPtr>& sq)
 {
	 auto it = s->getFirstItem();
	 rulePtr r = it.getRule();
	 size_t pos = it.getPos();
	 size_t size = r->rightHandLength();
	 symbolPtr first;
	 symbolPtr second;
	 if (pos < size) {
		 first = r->findNthElem(pos);
		 if (pos < size - 1)
			 second = r->findNthElem(pos + 1);
		 else
			 second = lookAhead;

		 if (first->isNonTerminal() && !second->isNonTerminal()) {
			 statusPtr st = constructNewStation(first, second, sm, sq);
			 constructNewEdge(s, st);
		 }
		 else if (first->isNonTerminal() && second->isNonTerminal()) {
			 assert(0);
			 // todo
		 }

	 }
 }

/*
 * subsetAlgorithm 子集构造算法.
 */
void FiniteStateAutomaton::subsetAlgorithm(statusPtr& station, map<wstring, statusPtr>& stationsMap, queue<statusPtr>& stations)
{
	auto rules = g_.findRules(station->getNextNonTerminal()); /* 首先生成第一个状态 */
	symbolPtr lookAhead = station->getLookAheadSymbol();
	queue<statusPtr> status;
	symbolPtr first;	
	symbolPtr second = lookAhead;	/* 用于记录下一个symbol */
	for (auto r : *rules) {
		first = r->findNthElem(0);
		statusPtr fs = makeNewStat(Item(r, 0, lookAhead));
		status.push(fs);
		constructNewEdge(station, fs);
		status2Station(fs, lookAhead, stationsMap, stations);
	}

	while (!status.empty()) {
		auto stat = status.front(); status.pop();
		statusPtr newStat;
		Item it = stat->getFirstItem();
		rulePtr r = it.getRule();
		size_t nextPos = it.getPos() + 1;
		size_t size = r->rightHandLength();
		
		if (nextPos <= size) { /* 构建新的状态 */
			newStat = makeNewStat(Item(r, nextPos, lookAhead));
			status.push(newStat);
			/* 添加新边 */
			symbolPtr currentSymbol = r->findNthElem(nextPos - 1);
			auto e = constructNewEdge(stat, newStat);
			e->matchContent = currentSymbol;
			status2Station(newStat, lookAhead, stationsMap, stations);
		}
	}
}

/*
 * constructNewStation 用于构建新的station
 */
statusPtr FiniteStateAutomaton::constructNewStation(symbolPtr& first, symbolPtr& second, map<wstring, statusPtr>& sm, queue<statusPtr>&sq)
{
	wstring key = first->uniqueMark() + second->uniqueMark();
	statusPtr st;
	auto pr = sm.find(key);
	if (pr == sm.end()) { /* 不曾出现过该station */
		st = makeNewStation(first, second);
		sm[key] = st;
		sq.push(st);
	}
	else 
		st = pr->second;
	return st;
}


edgePtr FiniteStateAutomaton::constructNewEdge(statusPtr& from, statusPtr& to)
{
	edgePtr e = makeNewEdge(from, to);
	from->outEdges.push_back(e);
	to->inEdges.push_back(e);
	return e;
}

/*
 * printGraph 用于输出所有的status和edge
 */
void FiniteStateAutomaton::printGraph() {
	wcout << L"<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
	wcout << L"Status:" << endl;
	for (auto pr : allStatus_) {
		auto status(pr.second.lock());
		if (status) {
			wcout << *status << endl;
		}
	}
	wcout << L"Edges:" << endl;
	for (auto pr : allEdges_) {
		auto edge(pr.second.lock());
		if (edge) {
			wcout << *edge << endl;
		}
	}
	wcout << L">>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
}

////////////////////////////////////////// Deterministic Automaton ///////////////////////////////////////////

/*
 * constructDeterministicAutomaton 构建确定性自动机.
 */
shared_ptr<ActionGoto> FiniteStateAutomaton::constructDeterministicAutomaton()
{
	auto head = constructNonDeterministicAutomaton();
	head = eliminateEps(head);
	shared_ptr<ActionGoto> table = make_shared<ActionGoto>(g_);
	queue<newStatusPtr> l;			/* 队列l */
	set<newStatusPtr, Compare> d;	/* 集合d */
	/* 首先将起始状态放入l和d中 */
	statusSetPtr oldStatusSet = make_shared<set<statusPtr>>();
	oldStatusSet->insert(head);	/* 从起始状态出发 */
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
					if (edge->matchContent == sm) {	/* 如果能够匹配的话 */
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
	//nfa_.clearEnv();
	//return table;
	//wcout << "nice!";
	return table;
}

//////////////////////////////////////// Eliminate Eps Edge////////////////////////////////////////////////////
/*
* getEffectEdges 用于获取所有的有效边.
*/
void FiniteStateAutomaton::getEffectEdges(set<statusPtr>& closure, set<edgePtr>& edges) {
	for (auto stat : closure) {
		for (auto edge : stat->outEdges) {
			if (!edge->isEps())
				edges.insert(edge);
		}
	}
}

/*
* epsClosure  求s节点的ε闭包,将结果放入closure之中.
*/
void FiniteStateAutomaton::epsClosure(statusPtr &s, set<statusPtr>& closure, set<edgePtr>& allEpsEdges, set<statusPtr>& visitedStatus) {
	visitedStatus.insert(s);  /* 首先需要标记该节点已经被访问过了. */
	for (auto i : s->outEdges) {	/* 遍历出边 */
		if (i->isEps()) {
			closure.insert(i->to);
			allEpsEdges.insert(i); /* 将所有的ε边全部收集起来,方便一起删除 */
			if (visitedStatus.find(i->to) == visitedStatus.end())
				epsClosure(i->to, closure, allEpsEdges, visitedStatus);
		}
	}
}

void FiniteStateAutomaton::fillContent(statusPtr &s, set<statusPtr>& sets)
{
	for (auto status : sets) {
		s->items.insert(status->items.begin(), status->items.end());
	}
}

/*
 * eliminateEps	用于消除ε边.
 */
statusPtr FiniteStateAutomaton::eliminateEps(statusPtr& head) {
	/* 第一步,得到所有的有效状态 */
	set<statusPtr> effectStatus;
	set<statusPtr> notEffectStatus;  /* 非有效的状态 */
	set<edgePtr> allEpsEdges;		 /* 用于存储所有的ε边 */
	for (auto pr : allStatus_) {
		statusPtr stat(pr.second.lock());
		if (stat) {
			bool effective = false;
			for (auto edge : stat->inEdges) {
				if (!edge->isEps()) {		/* 有效状态指的是存在非ε边输入的状态 */
					effectStatus.insert(stat);
					effective = true;
					break;
				}
			}
			if (!effective)
				notEffectStatus.insert(stat);
		}
		else {
			//	assert(0);
			cout << "Some status deconstructed!" << endl;
		}
	}
	effectStatus.insert(head);		/* 起始状态是一个有效状态 */
	notEffectStatus.erase(head);	/* 有可能将起始状态误判为了无效状态 */

	/* 第二步,对于每一个有效的状态,找到其的eps闭包 */
	vector<edgePtr> newAddedEdges;  /* 用于记录新添加的边 */
	set<edgePtr> effectEdges;
	set<edgePtr> backupEdges;
	for (statusPtr stat : effectStatus) {
		set<statusPtr> closure;
		epsClosure(stat, closure, allEpsEdges, set<statusPtr>());
		closure.erase(stat);
		if (closure.size() != 0)
			fillContent(stat, closure); /* 将closure里面状态的item转移到stat中 */
		/* 然后要从closure出发找到所有的边 */
		getEffectEdges(closure, effectEdges);
		for (auto edge : effectEdges) {
			backupEdges.insert(edge); /* 这些边虽然不是ε边,但是最终还是要删除的 */
			/* 这里正确的做法是复制一条边 */
			edgePtr e1;
			if (closure.find(edge->to) != closure.end()) {
				/* 如果该边指向的status属于stat的闭包,那么这条新建的边应该是从stat指向stat */
				e1 = makeNewEdge(stat, stat);
			}
			else {
				e1 = makeNewEdge(stat, edge->to);
			}
			newAddedEdges.push_back(e1);
			// 这里必须要提一点,那就是如果此时就在某个状态的outEdges里面添加新的边,
			// 如果该边也是有效边,而且该状态的检查在这个状态之后,会出现一些bug.所以
			// 不直接在这里将边加入到状态的的出边和入边列表中,等待检查做完之后,统一添加
			//stat->outEdges.push_back(e1);
			//edge->to->inEdges.push_back(e1);
			e1->matchContent = edge->matchContent; /* 拷贝内容 */
		}
		effectEdges.clear();
	}
	/* 统一将边添加到对应状态的出边和入边中 */
	for (auto edge : newAddedEdges) {
		edge->from->outEdges.push_back(edge);
		edge->to->inEdges.push_back(edge);
	}
	newAddedEdges.clear();
	/* 第三步,删除所有的ε边和无效状态 */
	/* 所有从有效状态出发的非ε边都不能删除,虽然从图上看,所有的边都被删除了,但是不排除特殊情况,因此,还是谨慎一点 */
	for (auto edge : backupEdges) { /* 这些边已经被拷贝了,可以删除 */
		removeElement(edge->from->outEdges, edge);
		removeElement(edge->to->inEdges, edge);
		edge->from.reset();
		edge->to.reset();
	}
	backupEdges.clear();
	/* 删除所有的ε边 */
	for (auto edge : allEpsEdges) {
		removeElement(edge->from->outEdges, edge);
		removeElement(edge->to->inEdges, edge);
		edge->from.reset();
		edge->to.reset();
	}
	allEpsEdges.clear();
	/* 删除所有的非有效状态 */
	for (auto stat : notEffectStatus) {
		stat->outEdges.clear();
		stat->inEdges.clear();
	}
	notEffectStatus.clear();
	//printGraph();
	return head;			/* 开始节点始终是开始节点 */
}
