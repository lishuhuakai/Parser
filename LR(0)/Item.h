#pragma once
#include "Rule.h"


/*
 * Item ���ڼ�¼һЩ״̬
 */
class Item
{
public:
	Item(rulePtr& r, int pos);
	~Item();
private:
	rulePtr rule_;		/* ���ڼ�¼�Ƶ����� */
	int pos_;			/* ���ڼ�¼Item�����Ѿ���������rule��right hand����һ����������. */
public:
	friend wostream& operator<<(wostream& os, Item& it);
	bool operator==(const Item& r) const {		/* ���ڱȽ�����Item�Ƿ���� */
		return (rule_ == r.rule_) && (pos_ == r.pos_);
	}
	int getPos() {
		return pos_;
	}
	bool operator<(const Item& r) const {	 /* ������������ò���,���ǻ��Ƿ��������. */
		if (rule_ != r.rule_)
			return rule_ < r.rule_;
		else
			return pos_ < r.pos_;
	}

	Item(const Item& r) {		/* �������캯�� */
		this->rule_ = r.rule_;
		this->pos_ = r.pos_;
	}
	
};
typedef shared_ptr<Item> itemPtr;

