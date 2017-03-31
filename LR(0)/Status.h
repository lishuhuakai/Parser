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
	int label;
	vector<edgePtr> inEdges;	 /* ���� */
	vector<edgePtr> outEdges;	 /* ���� */
	bool finalStatus;
	Status(int id) :
		finalStatus(false),
		label(id)
	{}
	Status(int id, Item& it) :
		finalStatus(false),
		label(id)
	{
		items.insert(it);
	}
	Item getFirstItem() { /* ���ص�һ��Item */
		return *items.begin();
	}
	set<Item> items;
};

/*
 * Edge ����������ͼ�Ĺ������ٲ��˱�.
 */
struct Edge {
	int label;
	vector<symbolPtr> matchContent;
	statusPtr from;
	statusPtr to;

	bool isEps() { /* �жϱ��Ƿ�Ϊ�ű� */
		return matchContent.size() == 0;
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
};

#endif // !STATUS_H_