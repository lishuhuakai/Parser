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
 * constructNonDeterministicAutomaton ΪLR(1)��������ȷ�����Զ���.
 */
statusPtr FiniteStateAutomaton::constructNonDeterministicAutomaton()
{
	map<wstring, statusPtr> stationsMap; /* ���ڼ�¼���е�station */
	queue<statusPtr> stations;
	symbolPtr first = g_.startSymbol_;
	symbolPtr second = Symbol::eof;
	statusPtr head = makeNewStation(first, second);
	stationsMap[first->uniqueMark() + second->uniqueMark()] = head; /* ���뵽stationsMap�� */
	stations.push(head);

	while (!stations.empty()) {
		auto station = stations.front(); stations.pop();  /* ȡ��һ��station */
		//wcout << *station << endl;
		subsetAlgorithm(station, stationsMap, stations);
	}
	//printGraph();
	return head;
}


/*
 * status2Station ��������status��statioon.
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
 * subsetAlgorithm �Ӽ������㷨.
 */
void FiniteStateAutomaton::subsetAlgorithm(statusPtr& station, map<wstring, statusPtr>& stationsMap, queue<statusPtr>& stations)
{
	auto rules = g_.findRules(station->getNextNonTerminal()); /* �������ɵ�һ��״̬ */
	symbolPtr lookAhead = station->getLookAheadSymbol();
	queue<statusPtr> status;
	symbolPtr first;	
	symbolPtr second = lookAhead;	/* ���ڼ�¼��һ��symbol */
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
		
		if (nextPos <= size) { /* �����µ�״̬ */
			newStat = makeNewStat(Item(r, nextPos, lookAhead));
			status.push(newStat);
			/* ����±� */
			symbolPtr currentSymbol = r->findNthElem(nextPos - 1);
			auto e = constructNewEdge(stat, newStat);
			e->matchContent = currentSymbol;
			status2Station(newStat, lookAhead, stationsMap, stations);
		}
	}
}

/*
 * constructNewStation ���ڹ����µ�station
 */
statusPtr FiniteStateAutomaton::constructNewStation(symbolPtr& first, symbolPtr& second, map<wstring, statusPtr>& sm, queue<statusPtr>&sq)
{
	wstring key = first->uniqueMark() + second->uniqueMark();
	statusPtr st;
	auto pr = sm.find(key);
	if (pr == sm.end()) { /* �������ֹ���station */
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
 * printGraph ����������е�status��edge
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
 * constructDeterministicAutomaton ����ȷ�����Զ���.
 */
shared_ptr<ActionGoto> FiniteStateAutomaton::constructDeterministicAutomaton()
{
	auto head = constructNonDeterministicAutomaton();
	head = eliminateEps(head);
	shared_ptr<ActionGoto> table = make_shared<ActionGoto>(g_);
	queue<newStatusPtr> l;			/* ����l */
	set<newStatusPtr, Compare> d;	/* ����d */
	/* ���Ƚ���ʼ״̬����l��d�� */
	statusSetPtr oldStatusSet = make_shared<set<statusPtr>>();
	oldStatusSet->insert(head);	/* ����ʼ״̬���� */
	table->appendNewStat(0, oldStatusSet); /* �����ʼ״̬ */
	newStatusPtr startPtr = make_shared<NewStatus>(oldStatusSet, assignNewStatusLabel());
	d.insert(startPtr);
	l.push(startPtr);

	while (!l.empty()) {
		/* �Ӷ�����ȡ��һ��״̬ */
		newStatusPtr s = l.front(); l.pop();
		/* ��������״̬����,�����յ��ַ��Ĳ��� */
		set<symbolPtr> symbols;
		getAllSymbols(s->oldStatusSet, symbols);

		for (auto sm : symbols) {	/* ����ÿһ���ַ� */
			statusSetPtr oldStatusSet = make_shared<set<statusPtr>>();
			bool finalTransfer = false;
			for (auto oldStatus : *s->oldStatusSet) {			/* ��������½ڵ��ÿ���ɽڵ� */
				for (auto edge : oldStatus->outEdges) {			/* ����ÿһ������ */
					if (edge->matchContent == sm) {	/* ����ܹ�ƥ��Ļ� */
						oldStatusSet->insert(edge->to);
					}
				}
			}
			/* ���е�������Ҫע���һ����,oldStatusSet������Ϊ�� */
			assert(oldStatusSet->size() != 0);
			newStatusPtr newStatus = make_shared<NewStatus>(oldStatusSet, 0);
			auto pos = d.find(newStatus);
			int toLabel = 0;
			if (pos != d.end()) { /* ���״̬�Ѿ��ڼ���D�д����� */
				toLabel = (*pos)->label;
			}
			else { /* ��������ڵĻ�,��Ҫ���·���һ��label */
				toLabel = assignNewStatusLabel();
				table->appendNewStat(toLabel, oldStatusSet);
				newStatus->label = toLabel;
				d.insert(newStatus);
				l.push(newStatus);
			}
			table->recordRelations(s->label, toLabel, sm);	/* ��ӹ�ϵ */
		}
	}
	//nfa_.clearEnv();
	//return table;
	//wcout << "nice!";
	return table;
}

//////////////////////////////////////// Eliminate Eps Edge////////////////////////////////////////////////////
/*
* getEffectEdges ���ڻ�ȡ���е���Ч��.
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
* epsClosure  ��s�ڵ�Ħűհ�,���������closure֮��.
*/
void FiniteStateAutomaton::epsClosure(statusPtr &s, set<statusPtr>& closure, set<edgePtr>& allEpsEdges, set<statusPtr>& visitedStatus) {
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

void FiniteStateAutomaton::fillContent(statusPtr &s, set<statusPtr>& sets)
{
	for (auto status : sets) {
		s->items.insert(status->items.begin(), status->items.end());
	}
}

/*
 * eliminateEps	���������ű�.
 */
statusPtr FiniteStateAutomaton::eliminateEps(statusPtr& head) {
	/* ��һ��,�õ����е���Ч״̬ */
	set<statusPtr> effectStatus;
	set<statusPtr> notEffectStatus;  /* ����Ч��״̬ */
	set<edgePtr> allEpsEdges;		 /* ���ڴ洢���еĦű� */
	for (auto pr : allStatus_) {
		statusPtr stat(pr.second.lock());
		if (stat) {
			bool effective = false;
			for (auto edge : stat->inEdges) {
				if (!edge->isEps()) {		/* ��Ч״ָ̬���Ǵ��ڷǦű������״̬ */
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
	effectStatus.insert(head);		/* ��ʼ״̬��һ����Ч״̬ */
	notEffectStatus.erase(head);	/* �п��ܽ���ʼ״̬����Ϊ����Ч״̬ */

	/* �ڶ���,����ÿһ����Ч��״̬,�ҵ����eps�հ� */
	vector<edgePtr> newAddedEdges;  /* ���ڼ�¼����ӵı� */
	set<edgePtr> effectEdges;
	set<edgePtr> backupEdges;
	for (statusPtr stat : effectStatus) {
		set<statusPtr> closure;
		epsClosure(stat, closure, allEpsEdges, set<statusPtr>());
		closure.erase(stat);
		if (closure.size() != 0)
			fillContent(stat, closure); /* ��closure����״̬��itemת�Ƶ�stat�� */
		/* Ȼ��Ҫ��closure�����ҵ����еı� */
		getEffectEdges(closure, effectEdges);
		for (auto edge : effectEdges) {
			backupEdges.insert(edge); /* ��Щ����Ȼ���Ǧű�,�������ջ���Ҫɾ���� */
			/* ������ȷ�������Ǹ���һ���� */
			edgePtr e1;
			if (closure.find(edge->to) != closure.end()) {
				/* ����ñ�ָ���status����stat�ıհ�,��ô�����½��ı�Ӧ���Ǵ�statָ��stat */
				e1 = makeNewEdge(stat, stat);
			}
			else {
				e1 = makeNewEdge(stat, edge->to);
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
	//printGraph();
	return head;			/* ��ʼ�ڵ�ʼ���ǿ�ʼ�ڵ� */
}
