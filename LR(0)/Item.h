#pragma once
#include "Rule.h"


/*
 * Item 用于记录一些状态
 */
class Item
{
public:
	Item(rulePtr& r, int pos);
	~Item();
private:
	rulePtr rule_;		/* 用于记录推导规则 */
	int pos_;			/* 用于记录Item现在已经解析到了rule的right hand的哪一个符号上了. */
public:
	friend wostream& operator<<(wostream& os, Item& it);
	bool operator==(const Item& r) const {		/* 用于比较两个Item是否相等 */
		return (rule_ == r.rule_) && (pos_ == r.pos_);
	}
	int getPos() {
		return pos_;
	}
	bool operator<(const Item& r) const {	 /* 这个东西可能用不到,但是还是放在这里吧. */
		if (rule_ != r.rule_)
			return rule_ < r.rule_;
		else
			return pos_ < r.pos_;
	}

	Item(const Item& r) {		/* 拷贝构造函数 */
		this->rule_ = r.rule_;
		this->pos_ = r.pos_;
	}
	
};
typedef shared_ptr<Item> itemPtr;

