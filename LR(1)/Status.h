#ifndef STATUS_H_
#define STATUS_H_

#include "common.h"
#include <memory>
#include "Item.h"
using namespace std;

struct Edge;
struct Status;
typedef shared_ptr<Edge> edgePtr;
typedef shared_ptr<Status> statusPtr;
typedef weak_ptr<Edge> edgeWeakPtr;
typedef weak_ptr<Status> statusWeakPtr;
typedef map<int, edgeWeakPtr> edgesCollector;
typedef map<int, statusWeakPtr> statusCollector;

struct Status {
	enum Kind {	kStatus, kStation};
	int label;
	set<Item> items;
	Kind kind;
	vector<edgePtr> inEdges;	 /* ���� */
	vector<edgePtr> outEdges;	 /* ���� */

	/* only for kStation */
	symbolPtr nonTerminal;		/* ���ս�� */
	symbolPtr lookAheadSymbol;	/* ǰ������ */

	Status(int id) :
		label(id),
		kind(kStatus)
	{}
	Status(int id, Item& it) :
		label(id),
		kind(kStatus)
	{
		items.insert(it);
	}

	Status(int id, symbolPtr& f, symbolPtr& s) :
		label(id), nonTerminal(f), lookAheadSymbol(s),
		kind(kStation)
	{}

	Item getFirstItem() { /* ���ص�һ��Item */
		return *items.begin();
	}

	symbolPtr getNextNonTerminal() {
		assert(this->kind == kStation);
		return nonTerminal;
	}

	symbolPtr getLookAheadSymbol() {
		assert(this->kind == kStation);
		return lookAheadSymbol;
	}

	friend wostream& operator<<(wostream&os, Status& s);
};


/*
 * Edge ����������ͼ�Ĺ������ٲ��˱�.
 */
struct Edge {
	int label;
	symbolPtr matchContent;
	statusPtr from;
	statusPtr to;

	bool isEps() { /* �жϱ��Ƿ�Ϊ�ű� */
		return matchContent == nullptr;
	}

	Edge(int id, const statusPtr& f, const statusPtr& t) :
		from(f), to(t), label(id)
	{}

	Edge(const Edge& e) {
		label = e.label;
		matchContent = e.matchContent;
		from = e.from;
		to = e.to;
	}
	friend wostream& operator<<(wostream& os, Edge& e);
};

#endif // !STATUS_H_