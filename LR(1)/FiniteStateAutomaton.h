#pragma once
#include "Common.h"
#include "Grammar.h"
#include "Status.h"

/*
 * FiniteStateAutomaton ����״̬��.
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
	struct NewStatus {				/* ������Ҫ����һ���µ����ݽṹ */
		statusSetPtr oldStatusSet;	/* ���ڼ�¼���е�״̬�ļ��� */
		int label;					/* ÿһ����״̬����һ����ǩ */
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
	Grammar& g_;		/* ���ڼ�¼�ķ� */
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
		/* o(n^2) Ч��ʵ�ڲ���ô��,�����Ż��ĵز�Ҳ���� */
		for (auto status : (*st)) {						/* ����ÿһ��״̬ */
			for (auto edge : (*status).outEdges) {		/* ����ÿһ������ */
				res.insert(edge->matchContent);
			}
		}
	}
	struct Compare {
		bool operator()(const newStatusPtr& l1, const newStatusPtr& l2) const
		{
			return *((*l1).oldStatusSet) > *((*l2).oldStatusSet); /* ֱ�ӱȽ�����ָ��Ƚϵ�����,������ָ���ֵ */
		}
	};
};

