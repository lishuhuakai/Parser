#ifndef EPSNFA_H_
#define EPSNFA_H_

#include "Status.h"
class Grammar;

class EpsNfa{
	friend class Nfa;
public:
	EpsNfa(Grammar& g);
	statusPtr buildEpsNfa();
	void printStatusAndEdges(const wstring& title);
private:
	void processStatus(statusPtr&, symbolPtr&, map<rulePtr, statusPtr>&);
	statusPtr makeNewNode(Item& it);
	edgePtr makeNewEdge(const statusPtr& from, const statusPtr& to);
	void cleanEnv();
	void clearAllEdges();
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
public:
	set<statusPtr> finalStatus_;
	Grammar& g_;		/* 用于记录文法 */
	statusCollector allStatus_;
	edgesCollector allEdges_;
};

#endif // !EPSNFA_H_
