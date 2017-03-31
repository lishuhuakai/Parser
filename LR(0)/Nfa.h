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
	statusPtr buildNfa(); /* 消除ε,构建NFA */
	void clearEnv(); /* 清理环境 */
private:
	EpsNfa& epsNfa_;
	edgesCollector allNewEdges_; /* 用于记录所有的新添加的边 */
	set<statusPtr> finalStatus_;  /* 用于记录最终的状态 */
private:
	void epsClosure(statusPtr&, set<statusPtr>&, set<edgePtr>&, set<statusPtr>&); /* 用于寻找一个status的ε闭包 */
	void getEffectEdges(set<statusPtr>&, set<edgePtr>&); /* 寻找闭包所有的有效边 */
	void printNewEdges(); /* 输出新边的信息 */
	void printFinalStat();
	void clearAllNewEdges();
	void fillContent(statusPtr&, set<statusPtr>&);
};

#endif //!NFA_H_