#pragma once
#include "Common.h"

/*
 * Item用于记录某些信息. 
 */
struct Item {
	rulePtr rule;		/* 用于记录规则 */
	int pos;			/* pos用于现在已经解析到rule的哪一个符号了. */
	int at;				/* 用于记录Item从哪个位置产生 */
	Item(rulePtr& r, int pos, int at) :
		rule(r), pos(pos), at(at)
	{}
	Item(const Item& it) { /* 拷贝构造函数 */
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
	vector<itemsPtr> itemSet_;	  /* itemSet集 */
	vector<itemsPtr> completed_;  /* 完成集 */
	grammarPtr& grammar_;  /* 用于记录所有的rule */
	int currentPos_;	  /* 记录当前所在的位置 */
private:
	itemsPtr predictor(itemsPtr& active, int pos);
	void distributor(itemsPtr& itemset, symbolPtr&, itemsPtr&, itemsPtr&);
	itemPtr makeItem(rulePtr r, int pos, int at) { /* 构建一条item */
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
	bool success(); /* 判断是否解析成功 */
};

