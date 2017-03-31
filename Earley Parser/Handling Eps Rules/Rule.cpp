#include "Rule.h"



Rule::Rule(symbolPtr& l) :
	left_(l)
{
}


Rule::~Rule()
{
}

/*
 * 拷贝构造函数
 */
Rule::Rule(const Rule & r)
{
	this->left_ = r.left_;
	this->right_ = r.right_;
}

bool Rule::operator==(Rule & r)
{
	if (*(this->left_) == *(r.left_)) {
		if (this->right_.size() != r.right_.size()) {
			return false;
		}
		for (size_t i = 0; i < right_.size(); ++i) {
			if (!(*(right_[i]) == *(r.right_[i])))
				return false;
		}
	}
	return true;
}

wostream & operator<<(wostream & os, Rule & r)
{
	os << *(r.left_) << L" --> ";
	for (auto elem : r.right_) {
		os << *elem;
	}
	if (r.right_.size() == 0)
		os << L"ɛ";
	os << endl;
	return os;
}
