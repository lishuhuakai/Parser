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

/*
 * 如果要开始处理ε规则的话,Parser将会变得更加复杂.
 */

bool EarleyParser::startParse(vector<symbolPtr>& str) {
	/* 首先是初始化的工作 */
	currentPos_ = 0;
	itemsPtr active = make_shared<vector<itemPtr>>();
	itemsPtr pred = make_shared<vector<itemPtr>>();
	itemsPtr completed = make_shared<vector<itemPtr>>();

	auto rules = grammar_->findRules(grammar_->startSymbol_);
	for (auto r : *rules) {
		if (r->isEpsRule()) {
			completed->push_back(makeItem(r, currentPos_, 1));
		}
		else
			active->push_back(makeItem(r, currentPos_, 1)); /* 刚开始的时候将从s出发的规则全部压进去 */
	}
	closure(active, pred, completed, false); /* 调用闭包算法,直到三者不再发生变化,false表示未曾调用过distributor函数 */
	wcout << L"itemset" << endl;
	auto itemset = combine(active, pred);
	printItems(itemset);
	itemSet_.push_back(itemset);
	completed_.push_back(completed); /* 插入一个空的completed1 */
	while (true) {
		if (currentPos_ == str.size()) {
			/* 什么时候会解析失败呢?当 */
			return success();
		}
		completed = make_shared<vector<itemPtr>>();
		active = make_shared<vector<itemPtr>>();
		pred = make_shared<vector<itemPtr>>();
		/*
		 * distributor函数的本质是什么,是扫描前一个itemset,将匹配的item advanced 1位,
		 * 放入active或者completed集中.
		 */
		distributor(itemSet_[currentPos_], str[currentPos_], completed, active);
		currentPos_++;
		closure(active, pred, completed, true);
		auto itemset = combine(active, pred);
		wcout << "itemset:" << endl;
		printItems(itemset);
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
	itemsPtr itemset = make_shared<vector<itemPtr>>();
	for (auto it : *active) {
		//insertIntoVectorIfNotExists(*itemset, it);
		itemset->push_back(it);
	}
	for (auto it : *pred) {
		insertIntoVectorIfNotExists(*itemset, it);
		//itemset->push_back(it);
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


/*
 * completer用于处理新填入completed的元素,也就是我们要处理的ε规则.
 * 
 */
size_t EarleyParser::completer(itemsPtr & completed, itemsPtr & active, size_t& completedPos)
{
	itemsPtr currentItemSet;		/* 用于记录当前的set */
	vector<itemPtr> newCompletedItems;
	vector<itemPtr> newActiveItems;
	/*
	* 只处理新添入的item.
	*/
	for (; completedPos < completed->size(); ++completedPos) {
		auto item = (*completed)[completedPos]; /* 获得新的item */
		/* 在这我要说明一下,新加入的item不光有这种形式的:
		 *  A -> ● @currentPos, 其中currentPos是当前正在处理的位置.
		 * 还可能有这种形式的:
		 * B -> XXX● @pos ,其中X既可以代表终结符也可以代表非终结符,pos小于currentPos
		 * 因此一旦出现了这种情况,那么要到itemset(pos)去寻找●B形式的未完成的item.
		 */
		if ((item->at - 1) < currentPos_) /* at一般比下标多1 */
			currentItemSet = itemSet_[item->at - 1];
		else
			currentItemSet = active;

		for (auto it : *currentItemSet) { /* 遍历currentSet,开始寻找 */
			symbolPtr symbol = it->rule->findNthElem(it->pos);
			if (*symbol == *(item->rule->startSymbol())) { /* 如果匹配的话要添加新的item到active中 */
				auto newItem = copyItem(it);
				newItem->pos++;
				/* 如果往前移动一步,到达了end,可以规约了,也要将其压入completed中 */
				if (newItem->pos == newItem->rule->length()) {
					/* 这种情况可以做优化,因为本来是处理新添加的completed项,但是处理之后依然有一个进入了completed中 */
					newCompletedItems.push_back(newItem);  /* 防止迭代器失效 */
				}
				else
					newActiveItems.push_back(newItem); /* 防止迭代器失效 */
			}
		}
	}

	for (auto item : newCompletedItems) {
		//completed->push_back(item);
		insertIntoVectorIfNotExists(*completed, item);
	}
	for (auto item : newActiveItems) {
		insertIntoVectorIfNotExists(*active, item);
		//active->push_back(item);
	}
	return active->size();
}
/*
 * 参数distributed用于表示是否调用过distributor函数.
 */
void EarleyParser::closure(itemsPtr &active, itemsPtr &pred, itemsPtr& completed, bool distributed)
{
	size_t activePos = 0;
	size_t completedPos = 0;
	/*
	 * 如果调用过distributor函数,说明completed里面的元素都已经被处理过了,所以completedPos要设置为completed->size()
	 * 否则的话,代表没有处理过,要从头开始处理.
	 */
	if (distributed)
		completedPos = completed->size();
	while (true) { /* 闭包算法,一直运行到两者不再发生变化为止 */
		/* predictor可能会导致completed大小发生变化 */
		if (predictor(active, pred, completed, activePos, currentPos_ + 1) != completedPos) {
			/* completer可能会导致completed和active发生变化 */
			if ((completer(completed, active, completedPos) == activePos) && (completedPos == completed->size()))
				break;
		}
		else break;
	}
	wcout << L"completed:" << endl;
	printItems(completed);
}


/*
 * predictor的工作开始变得有一些复杂了.
 */
size_t EarleyParser::predictor(itemsPtr& active, itemsPtr& pred, itemsPtr& completed, 
	size_t& activePos, int at) { /* 对规则进行预测 */

	list<symbolPtr> newSymbol; /* 我想尽量高效一点,不要动不动就遍历所有的规则 */
	/* 在active中不存在ε */ 
	for (; activePos < active->size(); ++activePos) {
		itemPtr item = (*active)[activePos];
		symbolPtr s = item->rule->findNthElem(item->pos); /* 找到对应位置的符号 */
		//wcout << *(item->rule);
		if (s->isNonTerminal()) /* 将非终结符号全部放入newSymbol中 */
			insertIntoListIfNotExists(newSymbol, s);
	}
	/* 事情还没有完成,对于pred中新引入的非终结符,也要扩展开来 */
	for (auto it = newSymbol.begin(); it != newSymbol.end(); ++it) {
		auto rules = grammar_->findRules(*it); /* 根据非终结符寻找规则 */
		for (auto rule : *rules) { /* 将规则压入pred */
			if (rule->isEpsRule()) { /* 如果是一条EPS规则的话,要将这条规则压入completed集合 */
				insertIntoVectorIfNotExists(*completed, makeItem(rule, 0, at));
				//completed->push_back();
			}
			else
			{
				symbolPtr s = rule->findNthElem(0);
				if (s->isNonTerminal())
					insertIntoListIfNotExists(newSymbol, s); /* newSymbol是在变化的,但是list的迭代器不会发生变化 */
				insertIntoVectorIfNotExists(*pred, makeItem(rule, 0, at));
				//pred->push_back();
			}
		}
	}
	//std::wcout << L"pred:" << endl;
	//printItems(pred);
	return completed->size();
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
	/*
	* distrubutor和completer两个函数在功能上很相近,是不是可以实现两个函数的合并呢?
	* 某些功能可以合并,但是不建议,因为会变得有些复杂,暂时先这样吧.
	*/
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
					completed->push_back(newItem);
					/* 这里需要做的一点是,如果newItem的at不是currentPos_,那么要另外处理 */
					int at = item->at - 1;
					if (at >= 0)
						insertIntoListIfNotExists(symbols, make_shared<CompletedItem>(item->rule->startSymbol(), at));
				}
				else { /* 否则的话,要放入active集合中 */
					active->push_back(newItem);
				}
			}
		}
	}
	//wcout << "completed:" << endl;
	//printItems(completed);
	//wcout << "active:" << endl;
	//printItems(active);
}

bool Item::operator==(Item & r)
{
	return (this->at == r.at) && (this->pos == r.pos) && (this->rule == r.rule);
}
