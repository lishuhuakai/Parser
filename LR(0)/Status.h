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
	vector<edgePtr> inEdges;	 /* 进边 */
	vector<edgePtr> outEdges;	 /* 出边 */
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
	Item getFirstItem() { /* 返回第一个Item */
		return *items.begin();
	}
	set<Item> items;
};

/*
 * Edge 遍历树生成图的过程中少不了边.
 */
struct Edge {
	int label;
	vector<symbolPtr> matchContent;
	statusPtr from;
	statusPtr to;

	bool isEps() { /* 判断边是否为ε边 */
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