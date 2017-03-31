#pragma once
#include "Common.h"
#include "Grammar.h"
#include "Status.h"

/*
 * FiniteStateAutomaton 有限状态机.
 */
class FiniteStateAutomaton
{
public:
	FiniteStateAutomaton(Grammar& g);
	~FiniteStateAutomaton();

private:
	void deleteStatusPtr(Status * s) {
		allStatus_.erase(s->label);
		delete s;
	}
	void deleteEdgePtr(Edge *e) {
		allEdges_.erase(e->label);
		delete e;
	}
	static int nextLabel() {
		static int count1 = 0;
		return count1++;
	}
	static int nextLabel2() {
		static int count2 = 0;
		return count2++;
	}
	int assignNewStatusLabel() {
		static int newStatusCount = 0;
		return newStatusCount++;
	}
private:
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
private:
	statusPtr makeNewStat(Item& it);
	statusPtr makeNewStation(symbolPtr&, symbolPtr&);
	edgePtr makeNewEdge(const statusPtr&, const statusPtr&);

private:
	Grammar& g_;		/* 用于记录文法 */
	statusCollector allStatus_;
	edgesCollector allEdges_;
private:
	void subsetAlgorithm(statusPtr&, map<wstring, statusPtr>&, queue<statusPtr>&);
	statusPtr constructNewStation(symbolPtr&, symbolPtr&, map<wstring, statusPtr>&, queue<statusPtr>&);
	void status2Station(statusPtr&, symbolPtr&, map<wstring, statusPtr>&, queue<statusPtr>&);
	edgePtr constructNewEdge(statusPtr& from, statusPtr& to);
public:
	statusPtr constructNonDeterministicAutomaton();
	shared_ptr<ActionGoto> constructDeterministicAutomaton();
	statusPtr eliminateEps(statusPtr&);
	void printGraph();
private:
	void getEffectEdges(set<statusPtr>& closure, set<edgePtr>& edges);
	void epsClosure(statusPtr &s, set<statusPtr>& closure, set<edgePtr>& allEpsEdges, set<statusPtr>& visitedStatus);
	void fillContent(statusPtr &s, set<statusPtr>& sets);
private:
	static void getAllSymbols(shared_ptr<set<statusPtr>>& st, set<symbolPtr>& res) {
		/* o(n^2) 效率实在不怎么样,但是优化的地步也不大 */
		for (auto status : (*st)) {						/* 对于每一个状态 */
			for (auto edge : (*status).outEdges) {		/* 对于每一条出边 */
				res.insert(edge->matchContent);
			}
		}
	}
	struct Compare {
		bool operator()(const newStatusPtr& l1, const newStatusPtr& l2) const
		{
			return *((*l1).oldStatusSet) > *((*l2).oldStatusSet); /* 直接比较两个指针比较的内容,而不是指针的值 */
		}
	};
};

