#pragma once
#include "Common.h"

/*
 * Item���ڼ�¼ĳЩ��Ϣ. 
 */
struct Item {
	rulePtr rule;		/* ���ڼ�¼���� */
	int pos;			/* pos���������Ѿ�������rule����һ��������. */
	int at;				/* ���ڼ�¼Item���ĸ�λ�ò��� */
	Item(rulePtr& r, int pos, int at) :
		rule(r), pos(pos), at(at)
	{}
	Item(const Item& it) { /* �������캯�� */
		rule = it.rule;
		pos = it.pos;
		at = it.at;
	}
	friend wostream& operator<<(wostream& os, Item& it);
};

class EarleyParser
{
	typedef shared_ptr<Item> itemPtr;
	typedef shared_ptr<set<itemPtr>> itemsPtr;
public:
	EarleyParser(grammarPtr&);
	~EarleyParser();
private:
	vector<itemsPtr> itemSet_;	  /* itemSet�� */
	vector<itemsPtr> completed_;  /* ��ɼ� */
	grammarPtr& grammar_;  /* ���ڼ�¼���е�rule */
	int currentPos_;	  /* ��¼��ǰ���ڵ�λ�� */
private:
	itemsPtr predictor(itemsPtr& active, int pos);
	void distributor(itemsPtr& itemset, symbolPtr&, itemsPtr&, itemsPtr&);
	itemPtr makeItem(rulePtr r, int pos, int at) { /* ����һ��item */
		return make_shared<Item>(r, pos, at);
	}
	itemPtr copyItem(itemPtr& it) {
		return make_shared<Item>(*it);
	}
	void printItems(itemsPtr&);
public:
	bool startParse(vector<symbolPtr>& str);
private:
	itemsPtr combine(itemsPtr& active, itemsPtr& pred);
	bool success(); /* �ж��Ƿ�����ɹ� */
};

