#include "Dfa.h"
#include "Status.h"
#include "Nfa.h"
#include "ActionGoto.h"

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

struct Compare {
	bool operator()(const newStatusPtr& l1, const newStatusPtr& l2) const
	{
		return *((*l1).oldStatusSet) > *((*l2).oldStatusSet); /* ֱ�ӱȽ�����ָ��Ƚϵ�����,������ָ���ֵ */
	}
};


Dfa::Dfa(Nfa& n) :
	nfa_(n),
	builted(false)
{
}


Dfa::~Dfa()
{
}


shared_ptr<ActionGoto> Dfa::buildDFA() {
	shared_ptr<ActionGoto> table = make_shared<ActionGoto>();
	statusPtr start = nfa_.buildNfa();
	queue<newStatusPtr> l;			/* ����l */
	set<newStatusPtr, Compare> d;	/* ����d */
	/* ���Ƚ���ʼ״̬����l��d�� */
	statusSetPtr oldStatusSet = make_shared<set<statusPtr>>();
	oldStatusSet->insert(start);	/* ����ʼ״̬���� */
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
					if (findElement(edge->matchContent, sm)) {	/* ����ܹ�ƥ��Ļ� */
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
	nfa_.clearEnv();
	table->buildTable();
	return table;
}


void Dfa::getAllSymbols(shared_ptr<set<statusPtr>>& st, set<symbolPtr>& res) {
	/* o(n^3) Ч��ʵ�ڲ���ô��,�����Ż��ĵز�Ҳ���� */
	for (auto status : (*st)) {						/* ����ÿһ��״̬ */
		for (auto edge : (*status).outEdges) {		/* ����ÿһ������ */
			for (auto i : edge->matchContent)
				res.insert(i);
		}
	}
}

/*
 * assignNewStatusLabel ���ڷ���һ���µı��
 */
int Dfa::assignNewStatusLabel() { 
	static int newStatusCount = 0;
	return newStatusCount++;
}