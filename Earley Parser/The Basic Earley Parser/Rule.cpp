#include "Rule.h"



Rule::Rule(symbolPtr& l) :
	left_(l)
{
}


Rule::~Rule()
{
}

/*
 * �������캯��
 */
Rule::Rule(const Rule & r)
{
	this->left_ = r.left_;
	this->right_ = r.right_;
}

wostream & operator<<(wostream & os, Rule & r)
{
	os << *(r.left_) << L" --> ";
	for (auto elem : r.right_) {
		os << *elem;
	}
	os << endl;
	return os;
}
