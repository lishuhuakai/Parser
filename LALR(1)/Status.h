#ifndef STATUS_H_
#define STATUS_H_

#include "common.h"
#include "Rule.h"
#include <memory>
using namespace std;

struct Status;
typedef shared_ptr<Status> statusPtr;

/*
 * status ��ʵ����һ��Ƕ���.
 */
struct Status {
	shared_ptr<set<Item>> items;  /* ���ڼ�¼һ��Ʊ��Item */

	Status()
	{
		items = make_shared<set<Item>>();
	}
	          
	void insertItem(Item& it) {
		items->insert(it);
	}
	friend wostream& operator<<(wostream&os, Status& s);
};

/*
 * Ϊ�˾������ٱȽ�,�������һ��hashֵ.
 */
struct StatusHash {
	size_t operator() (Status const& s) const {
		size_t res = 1;
		for (auto it : *s.items) {
			res = res ^ (it.hash() << 1);
		}
		return res;
	}
};

#endif // !STATUS_H_