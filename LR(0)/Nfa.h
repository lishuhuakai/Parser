#ifndef NFA_H_
#define NFA_H_

#include "common.h"
#include "EpsNfa.h"

class Match;

class Nfa
{
public:
	Nfa(EpsNfa& epsNfa);
	~Nfa();
public:
	friend class Match;
	statusPtr buildNfa(); /* ������,����NFA */
	void clearEnv(); /* ������ */
private:
	EpsNfa& epsNfa_;
	edgesCollector allNewEdges_; /* ���ڼ�¼���е�����ӵı� */
	set<statusPtr> finalStatus_;  /* ���ڼ�¼���յ�״̬ */
private:
	void epsClosure(statusPtr&, set<statusPtr>&, set<edgePtr>&, set<statusPtr>&); /* ����Ѱ��һ��status�Ħűհ� */
	void getEffectEdges(set<statusPtr>&, set<edgePtr>&); /* Ѱ�ұհ����е���Ч�� */
	void printNewEdges(); /* ����±ߵ���Ϣ */
	void printFinalStat();
	void clearAllNewEdges();
	void fillContent(statusPtr&, set<statusPtr>&);
};

#endif //!NFA_H_