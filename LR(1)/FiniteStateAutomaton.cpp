#include "FiniteStateAutomaton.h"
#include <functional>


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
		wcout << *station << endl;
		subsetAlgorithm(station, stationsMap, stations);
	}
	return head;
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
		if (first->isNonTerminal()) {
			statusPtr st = constructNewStation(first, second, stationsMap, stations);
			constructNewEdge(fs, st);	/* 构成ε边 */
		}
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
			e->matchContent.push_back(currentSymbol);
			if (nextPos < size) { /* 在pos之后至少存在两个符号 */
				if (nextPos < size - 1) {
					first = r->findNthElem(nextPos);
					second = r->findNthElem(nextPos + 1);
				}
				else {
					first = r->findNthElem(nextPos);
					second = lookAhead;
				}
				if (first->isNonTerminal() && second->isTerminal()) { /* 需要构建station */
					statusPtr st = constructNewStation(first, second, stationsMap, stations);
					constructNewEdge(newStat, st);
				}
			}
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

 void FiniteStateAutomaton::status2Station(rulePtr &, int, map<wstring, statusPtr>&, queue<statusPtr>&)
 {
 }

edgePtr FiniteStateAutomaton::constructNewEdge(statusPtr& from, statusPtr& to)
{
	edgePtr e = makeNewEdge(from, to);
	from->outEdges.push_back(e);
	to->inEdges.push_back(e);
	return e;
}


