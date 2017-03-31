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
 * getEffectEdges ���ڻ�ȡ���е���Ч��.
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
 * epsClosure  ��s�ڵ�Ħűհ�,���������closure֮��.
 */
void Nfa::epsClosure(statusPtr &s, set<statusPtr>& closure, set<edgePtr>& allEpsEdges, set<statusPtr>& visitedStatus) {
	visitedStatus.insert(s);  /* ������Ҫ��Ǹýڵ��Ѿ������ʹ���. */
	for (auto i : s->outEdges) {	/* �������� */
		if (i->isEps()) {
			closure.insert(i->to);
			allEpsEdges.insert(i); /* �����еĦű�ȫ���ռ�����,����һ��ɾ�� */
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
	/* ��һ��,�õ����е���Ч״̬ */
	set<statusPtr> effectStatus;
	set<statusPtr> notEffectStatus;  /* ����Ч��״̬ */
	set<edgePtr> allEpsEdges;  /* ���ڴ洢���еĦű� */
	for (auto pr : epsNfa_.allStatus_) {
		statusPtr stat(pr.second.lock());
		if (stat) {
			bool effective = false;
			for (auto edge : stat->inEdges) {
				if (!edge->isEps()) { /* ��Ч״ָ̬���Ǵ��ڷǦű������״̬ */
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
	effectStatus.insert(head); /* ��ʼ״̬��һ����Ч״̬ */
	notEffectStatus.erase(head); /* �п��ܽ���ʼ״̬����Ϊ����Ч״̬ */

	/* �ڶ���,����ÿһ����Ч��״̬,�ҵ����eps�հ� */
	vector<edgePtr> newAddedEdges;  /* ���ڼ�¼����ӵı� */
	set<edgePtr> effectEdges;
	set<edgePtr> backupEdges;
	for (statusPtr stat : effectStatus) {
		set<statusPtr> closure;
		epsClosure(stat, closure, allEpsEdges, set<statusPtr>());
		closure.erase(stat);
		if (closure.size() != 0)
			fillContent(stat, closure); /* ��closure����״̬��itemת�嵽stat�� */
		/* Ȼ��Ҫ��closure�����ҵ����еı� */
		getEffectEdges(closure, effectEdges);
		for (auto edge : effectEdges) {
			backupEdges.insert(edge); /* ��Щ����Ȼ���Ǧű�,�������ջ���Ҫɾ���� */
			/* ������ȷ�������Ǹ���һ���� */
			edgePtr e1;
			if (closure.find(edge->to) != closure.end()) {
				/* ����ñ�ָ���status����stat�ıհ�,��ô�����½��ı�Ӧ���Ǵ�statָ��stat */
				e1 = epsNfa_.makeNewEdge(stat, stat);
			}
			else {
				e1 = epsNfa_.makeNewEdge(stat, edge->to);
			}
			newAddedEdges.push_back(e1);
			// �������Ҫ��һ��,�Ǿ��������ʱ����ĳ��״̬��outEdges��������µı�,
			// ����ñ�Ҳ����Ч��,���Ҹ�״̬�ļ�������״̬֮��,�����һЩbug.����
			// ��ֱ�������ｫ�߼��뵽״̬�ĵĳ��ߺ�����б���,�ȴ��������֮��,ͳһ���
			//stat->outEdges.push_back(e1);
			//edge->to->inEdges.push_back(e1);
			e1->matchContent = edge->matchContent; /* �������� */
		}
		effectEdges.clear();
	}
	/* ͳһ������ӵ���Ӧ״̬�ĳ��ߺ������ */
	for (auto edge : newAddedEdges) {
		edge->from->outEdges.push_back(edge);
		edge->to->inEdges.push_back(edge);
	}
	newAddedEdges.clear();
	/* ������,ɾ�����еĦűߺ���Ч״̬ */
	/* ���д���Ч״̬�����ķǦű߶�����ɾ��,��Ȼ��ͼ�Ͽ�,���еı߶���ɾ����,���ǲ��ų��������,���,���ǽ���һ�� */
	for (auto edge : backupEdges) { /* ��Щ���Ѿ���������,����ɾ�� */
		removeElement(edge->from->outEdges, edge);
		removeElement(edge->to->inEdges, edge);
		edge->from.reset();
		edge->to.reset();
	}
	backupEdges.clear();
	/* ɾ�����еĦű� */
	for (auto edge : allEpsEdges) {
		removeElement(edge->from->outEdges, edge);
		removeElement(edge->to->inEdges, edge);
		edge->from.reset();
		edge->to.reset();
	}
	allEpsEdges.clear();
	/* ɾ�����еķ���Ч״̬ */
	for (auto stat : notEffectStatus) {
		stat->outEdges.clear();
		stat->inEdges.clear();
	}
	notEffectStatus.clear();
	//printNewEdges();
	return head;			/* ��ʼ�ڵ�ʼ���ǿ�ʼ�ڵ� */
}



/*
 * clearEnv ���������Ҫ��Ϊ�˱����ڴ�й¶,Ҫ�����е�Node�Լ�Edgeȫ��������.
 */
void Nfa::clearEnv() {
	epsNfa_.cleanEnv();
	clearAllNewEdges(); /* ������ӵı�ȫ��ɾ���� */
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
