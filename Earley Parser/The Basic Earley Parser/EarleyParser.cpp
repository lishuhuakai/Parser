#include "EarleyParser.h"
#include "Common.h"
#include "Grammar.h"
#include "Rule.h"

/*
 * Item的operator<<函数纯粹是为了调试方便.
 */
wostream & operator<<(wostream & os, Item & it)
{
	os << *(it.rule->startSymbol()) << " --> ";
	size_t i = 0;
	size_t size = it.rule->length();
	for ( ; i < size; ++i) {
		if (i == it.pos)
			os << L"●";
		os << *(it.rule->findNthElem(i));
	}
	if (i == it.pos)
		os << L"●";
	os << L"	@" << it.at << endl;
	return os;
}

EarleyParser::EarleyParser(grammarPtr& g) :
	grammar_(g), currentPos_(0)
{
}


EarleyParser::~EarleyParser()
{
}


bool EarleyParser::startParse(vector<symbolPtr>& str) {
	/* 首先是初始化的工作 */
	currentPos_ = 0;
	itemsPtr active = make_shared<set<itemPtr>>();
	auto rules = grammar_->findRules(grammar_->startSymbol_);
	for (auto r : *rules) {
		active->insert(makeItem(r, currentPos_, 1));
	}
	auto pred = predictor(active, currentPos_+1); /* 得到预测集 */
	itemSet_.push_back(combine(active, pred));
	completed_.push_back(make_shared<set<itemPtr>>()); /* 插入一个空的completed1 */
	while (true) {
		if (currentPos_ == str.size()) {
			/* 什么时候会解析失败呢?当 */
			return success();
		}
		itemsPtr completed = make_shared<set<itemPtr>>();
		active = make_shared<set<itemPtr>>();
		distributor(itemSet_[currentPos_], str[currentPos_], completed, active);
		currentPos_++;
		pred = predictor(active, currentPos_ + 1);
		auto itemset = combine(active, pred);
		if (itemset->size() == 0) {
			return false;
		}
		itemSet_.push_back(itemset);
		completed_.push_back(completed);
	}
	return false;
}

/*
 * combine 将active和pred两个集合合并,返回一个itemset.
 */
EarleyParser::itemsPtr EarleyParser::combine(itemsPtr & active, itemsPtr & pred)
{
	itemsPtr itemset = make_shared<set<itemPtr>>();
	for (auto it : *active) {
		itemset->insert(it);
	}
	for (auto it : *pred) {
		itemset->insert(it);
	}
	return itemset;
}

/*
 * success 用于判断是否解析成功.
 */
bool EarleyParser::success()
{
	for (auto item : *completed_.back()) { /* 只需要查看最后一个completed集合即可 */
		if (*(item->rule->startSymbol()) == *(grammar_->startSymbol_) && item->at == 1) { /* 找到了开始符号 */
			return true;
		}
	}
	return false;
}

void EarleyParser::printItems(itemsPtr& it) {
	for (auto i : *it) {
		wcout << *i;
	}
}

EarleyParser::itemsPtr EarleyParser::predictor(itemsPtr& active, int at) { /* 对规则进行预测 */
	itemsPtr pred = make_shared<set<itemPtr>>();
	list<symbolPtr> newSymbol; /* 我想尽量高效一点,不要动不动就遍历所有的规则 */
	for (auto r : *active) {
		symbolPtr s = r->rule->findNthElem(r->pos); /* 找到对应位置的符号 */
		//wcout << *r;
		if (s->isNonTerminal())
			insertIntoListIfNotExists(newSymbol, s);
	}
	/* 事情还没有完成,对于pred中新引入的非终结符,也要扩展开来 */
	for (auto it = newSymbol.begin(); it != newSymbol.end(); ++it) {
		auto rules = grammar_->findRules(*it);
		for (auto rule : *rules) { /* 将规则压入pred */
			//wcout << *rule;
			symbolPtr s = rule->findNthElem(0);
			if (s->isNonTerminal())
				insertIntoListIfNotExists(newSymbol, s); /* newSymbol是在变化的,但是list的迭代器不会发生变化 */
			pred->insert(makeItem(rule, 0, at));
		}
	}
	wcout << "pred:" << endl;
	printItems(pred);
	return pred;
}


struct CompletedItem {
	symbolPtr s;	/* 用于标记符号 */
	int at;			/* 起始的位置 */
	friend bool operator==(CompletedItem& l, CompletedItem& r) {
		return *(l.s) == *(r.s) && l.at == r.at;
	}
	CompletedItem(symbolPtr& s, int at) :
		s(s), at(at)
	{}
};
typedef shared_ptr<CompletedItem> completedItemPtr;
/*
 * distributor 检查itemset,实现分发的功能.
 */
void EarleyParser::distributor(itemsPtr & itemset, symbolPtr& ch, itemsPtr& completed, itemsPtr& active)
{
	list<completedItemPtr> symbols;
	symbols.push_back(make_shared<CompletedItem>(ch, currentPos_));
	itemsPtr currentSet;
	for (auto s : symbols) {
		/* 检查每一个Item,找到匹配项 */
		currentSet = itemSet_[s->at];
		for (auto item : *currentSet) {
			symbolPtr symbol = item->rule->findNthElem(item->pos);
			if (*symbol == *(s->s)) { /* 碰到了相匹配的字符 */
				auto newItem = copyItem(item);
				newItem->pos++;
				if (newItem->pos == newItem->rule->length()) { /* 这条规则完成,可以规约了 */
					completed->insert(newItem);
					/* 这里需要做的一点是,如果newItem的at不是currentPos_,那么要另外处理 */
					int at = item->at - 1;
					if (at >= 0)
						insertIntoListIfNotExists(symbols, make_shared<CompletedItem>(item->rule->startSymbol(), at));
				}
				else { /* 否则的话,要放入active集合中 */
					active->insert(newItem);
				}
			}
		}
	}
	wcout << "completed:" << endl;
	printItems(completed);
	wcout << "active:" << endl;
	printItems(active);
}

