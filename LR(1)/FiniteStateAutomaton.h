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
	void status2Station(rulePtr&, int, map<wstring, statusPtr>&, queue<statusPtr>&);
	edgePtr constructNewEdge(statusPtr& from, statusPtr& to);
public:
	statusPtr constructNonDeterministicAutomaton();
};

