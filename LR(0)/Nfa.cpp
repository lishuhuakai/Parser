#include "Nfa.h"
#include "EpsNfa.h"


Nfa::Nfa(EpsNfa& epsNfa) :
	epsNfa_(epsNfa)
{
}

Nfa::~Nfa()
{
}

/*
 * getEffectEdges 用于获取所有的有效边.
 */
void Nfa::getEffectEdges(set<statusPtr>& closure, set<edgePtr>& edges) {
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
void Nfa::epsClosure(statusPtr &s, set<statusPtr>& closure, set<edgePtr>& allEpsEdges, set<statusPtr>& visitedStatus) {
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

void Nfa::fillContent(statusPtr &s, set<statusPtr>& sets)
{
	for (auto status : sets) {
		s->items.insert(status->items.begin(), status->items.end());
	}
}

statusPtr Nfa::buildNfa() {
	statusPtr head = epsNfa_.buildEpsNfa();
	finalStatus_.insert(epsNfa_.finalStatus_.begin(), epsNfa_.finalStatus_.end());
	/* 第一步,得到所有的有效状态 */
	set<statusPtr> effectStatus;
	set<statusPtr> notEffectStatus;  /* 非有效的状态 */
	set<edgePtr> allEpsEdges;  /* 用于存储所有的ε边 */
	for (auto pr : epsNfa_.allStatus_) {
		statusPtr stat(pr.second.lock());
		if (stat) {
			bool effective = false;
			for (auto edge : stat->inEdges) {
				if (!edge->isEps()) { /* 有效状态指的是存在非ε边输入的状态 */
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
	effectStatus.insert(head); /* 起始状态是一个有效状态 */
	notEffectStatus.erase(head); /* 有可能将起始状态误判为了无效状态 */

	/* 第二步,对于每一个有效的状态,找到其的eps闭包 */
	vector<edgePtr> newAddedEdges;  /* 用于记录新添加的边 */
	set<edgePtr> effectEdges;
	set<edgePtr> backupEdges;
	for (statusPtr stat : effectStatus) {
		set<statusPtr> closure;
		epsClosure(stat, closure, allEpsEdges, set<statusPtr>());
		closure.erase(stat);
		if (closure.size() != 0)
			fillContent(stat, closure); /* 将closure里面状态的item转义到stat中 */
		/* 然后要从closure出发找到所有的边 */
		getEffectEdges(closure, effectEdges);
		for (auto edge : effectEdges) {
			backupEdges.insert(edge); /* 这些边虽然不是ε边,但是最终还是要删除的 */
			/* 这里正确的做法是复制一条边 */
			edgePtr e1;
			if (closure.find(edge->to) != closure.end()) {
				/* 如果该边指向的status属于stat的闭包,那么这条新建的边应该是从stat指向stat */
				e1 = epsNfa_.makeNewEdge(stat, stat);
			}
			else {
				e1 = epsNfa_.makeNewEdge(stat, edge->to);
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
	//printNewEdges();
	return head;			/* 开始节点始终是开始节点 */
}



/*
 * clearEnv 这个函数主要是为了避免内存泄露,要将所有的Node以及Edge全部析构掉.
 */
void Nfa::clearEnv() {
	epsNfa_.cleanEnv();
	clearAllNewEdges(); /* 将新添加的边全部删除掉 */
}

void Nfa::printNewEdges() {
	epsNfa_.printStatusAndEdges(L"Nfa");
}

void Nfa::clearAllNewEdges() {
	for (auto pr = allNewEdges_.begin(); pr != allNewEdges_.end(); ) {
		auto oldPr = pr++;
		edgePtr ePtr((*oldPr).second.lock());
		if (ePtr) {
			removeElement(ePtr->from->outEdges, ePtr);
			removeElement(ePtr->to->inEdges, ePtr);
			ePtr->from.reset();
			ePtr->to.reset();
		}
	}
}


void Nfa::printFinalStat() {
	cout << "Final Stats:" << endl;
	for (auto s : finalStatus_) {
		cout << s->label << endl;
	}
}
