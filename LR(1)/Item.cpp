#include "Item.h"



Item::Item(rulePtr& r, int pos, symbolPtr& lookahead) :
	rule_(r), pos_(pos), lookAhead_(lookahead)
{
}


Item::~Item()
{
}

/*
 * Item的operator<<函数纯粹是为了调试方便.
 */
wostream & operator<<(wostream & os, Item & it)
{
	os << *(it.rule_->leftHandSymbol()) << L" --> ";
	size_t i = 0;
	size_t size = it.rule_->rightHandLength();
	for ( ; i < size; ++i) {
		if (i == it.pos_)
			os << L"●";
		os << *(it.rule_->findNthElem(i));
	}
	if (i == it.pos_)
		os << L"●";
	return os;
}

