#pragma once
#include "Rule.h"


/*
 * Item ���ڼ�¼һЩ״̬
 */
class Item
{
public:
	Item(rulePtr&, int, symbolPtr&);
	~Item();
private:
	rulePtr rule_;		/* ���ڼ�¼�Ƶ����� */
	int pos_;			/* ���ڼ�¼Item�����Ѿ���������rule��right hand����һ����������. */
	symbolPtr lookAhead_;  /* ǰ������ */
public:
	friend wostream& operator<<(wostream& os, Item& it);
	bool operator==(const Item& r) const {		/* ���ڱȽ�����Item�Ƿ���� */
		return (rule_ == r.rule_) && (pos_ == r.pos_) && (lookAhead_ == r.lookAhead_);
	}
	int getPos() {
		return pos_;
	}
	rulePtr getRule() {
		return rule_;
	}
	bool operator<(const Item& r) const {	 /* ������������ò���,���ǻ��Ƿ��������. */
		if (rule_ != r.rule_)
			return rule_ < r.rule_;
		else if (pos_ != r.pos_)
			return pos_ < r.pos_;
		else
			return lookAhead_ < r.lookAhead_;
	}

	Item(const Item& r) {		/* �������캯�� */
		this->rule_ = r.rule_;
		this->pos_ = r.pos_;
		this->lookAhead_ = r.lookAhead_;
	}
	
};
typedef shared_ptr<Item> itemPtr;

