#include "Grammar.h"
#include "Rule.h"
#include "Symbol.h"


Grammar::Grammar(symbolPtr& s) :
	startSymbol_(s)
{
}


Grammar::~Grammar()
{
}

/*
 * findRules 用于寻找非终结符l的推导规则.
 */
shared_ptr<vector<rulePtr>> Grammar::findRules(symbolPtr & l)
{
	assert(l->type_ == Symbol::NonTerminal);
	shared_ptr<vector<rulePtr>> result = make_shared<vector<rulePtr>>();
	for (auto rule : rules_) {
		if (rule->leftHandEqualTo(l)) {
			result->push_back(rule);
		}
	}
	return result;
}

/*
 * calcFirstSet 计算所有非终结符号的FIRST集,这是一个闭包算法,迭代次数可能会有一点多. 
 */
void Grammar::calcFirstSet()
{
	firstSets_.clear();
	/* 第一步要做的是,将所有的first集合设置为空. */
	for (auto nt : nonTerminals_) {
		firstSets_[nt] = make_shared<symbolSet>();
	}
	/* 接下来开始迭代 */
	while (true) {
		bool changed = false;
		for (auto r : rules_) {
			symbolSetPtr firstSet = firstSets_[r->leftHandSymbol()];
			size_t size = firstSet->size();
			/* 每次插入完毕的时候都需要检测是否改变了,所以干脆将这个玩意写成宏 */
#define DETECT_WEATHER_CHANGED(x)								\
do {															\
			if (x != firstSet->size()) {						\
				changed = true;									\
			}													\
} while(0)														

			if (r->isEpsRule()) {
				firstSet->insert(Symbol::eps);
				DETECT_WEATHER_CHANGED(size);
			}
			else {
				symbolPtr rightHandFirstElemt = r->findNthElem(0);	/* 得到右侧的第一个元素 */
				symbolPtr leftHandSymbol = r->leftHandSymbol();

				if (rightHandFirstElemt->isNonTerminal()) {
					for (size_t i = 0; i < r->rightHandLength(); ++i) {
						symbolPtr rightHandElemt = r->findNthElem(i);
						if (rightHandElemt->isTerminal()) {
							firstSet->insert(rightHandFirstElemt);
							DETECT_WEATHER_CHANGED(size);
							break;
						}
						bool shouldBreak = true;
						for (auto symbol : *firstSets_[rightHandElemt]) {
							if (!symbol->isEps())
								firstSet->insert(symbol);
							else
								shouldBreak = false;
						}
						DETECT_WEATHER_CHANGED(size);
						if (shouldBreak) {
							/* 如果并没有在这个非终结符的FIRST集找到ε,那么要停止 */
							break;
						}
					}
				}
				else if (rightHandFirstElemt->isTerminal()) {		/* 非终结符直接插入 */
					firstSet->insert(rightHandFirstElemt);
					DETECT_WEATHER_CHANGED(size);
				}
			}
		}
		//printFirstSet();
		//wcout << L"--------------------" << endl;
		if (!changed)  /* 需要一直迭代到不再发生变化 */
			break;
	}
}

void Grammar::printMap(map<symbolPtr, symbolSetPtr>& mp)
{
	for (auto fs : mp) {
		wcout << *(fs.first) << L":" << endl;
		for (auto elemt : *(fs.second)) {
			wcout << *elemt << L" ";
		}
		wcout << endl;
	}
}

void Grammar::printMap(map<symbolPtr, bool>& mp) {
	for (auto fs : mp) {
		wcout << *(fs.first) << L":" << fs.second;
		wcout << endl;
	}
}

/*
 * calcFollowSet 计算所有非终结符号的FOLLOW集,同样也是一个闭包算法.
 */
void Grammar::calcFollowSet()
{
	followSets_.clear();
	/* 第一步要做的是,将所有的first集合设置为空. */
	for (auto nt : nonTerminals_) {
		followSets_[nt] = make_shared<symbolSet>();
	}
	confirmCouldProductEmptyStr();		/* 确定哪一些非终结符可以产生空串,只计算一次就行了. */
	if (firstSets_.size() == 0)
		calcFirstSet();
	//printMap(firstSets_);
	//wcout << L"______firstSet--------" << endl;
	followSets_[startSymbol_]->insert(Symbol::eof);  /* 开始符号的follow集中一定存在终止符号 */
	while (true) {
		bool changed = false;
		for (auto r : rules_) {
			size_t size = r->rightHandLength();
			size_t prevSize;  /* 用于记录当前集合的大小 */
			symbolSetPtr currentFollowSet;

#define CHECK_WEATHER_CURRENTFOLLOWSET_CHANGED()		\
do {													\
			if (prevSize != currentFollowSet->size())	\
				changed = true;							\
}while(0)

			if (!r->isEpsRule() && size == 1) { // A -> x 这种形式
				auto rightHandFirstElemt = r->findNthElem(0);
				currentFollowSet = followSets_[rightHandFirstElemt];
				prevSize = currentFollowSet->size();
				currentFollowSet->insert(followSets_[r->leftHandSymbol()]->begin(), followSets_[r->leftHandSymbol()]->end());
				CHECK_WEATHER_CURRENTFOLLOWSET_CHANGED();
			}
			else if (size > 1) {
				auto currentSymbol = r->findNthElem(0);
				symbolPtr nextSymbol;
				
				for (size_t i = 1; i < size; ++i) {
					nextSymbol = r->findNthElem(i);
					if (currentSymbol->isTerminal()) { /* 不处理非终结符 */
						currentSymbol = nextSymbol;
						continue;
					}
					currentFollowSet = followSets_[currentSymbol];
					prevSize = currentFollowSet->size();

					if (currentSymbol->isNonTerminal() && nextSymbol->isNonTerminal()) {
						for (auto symbol : *firstSets_[nextSymbol]) {
							if (!symbol->isEps())  /* 处理eps,其余全部加入currentFollowSet */
								currentFollowSet->insert(symbol);
						}
						if (productEmptyStr_[nextSymbol]) {	/* 如果下一个非终结符可以产生空串,那么要将nextSymbol的follow集加入currentSymbol的follow集 */
							currentFollowSet->insert(followSets_[nextSymbol]->begin(), followSets_[nextSymbol]->end());
						}
						CHECK_WEATHER_CURRENTFOLLOWSET_CHANGED();
					}
					else if (currentSymbol->isNonTerminal() && nextSymbol->isTerminal()) {
						currentFollowSet->insert(nextSymbol);
						CHECK_WEATHER_CURRENTFOLLOWSET_CHANGED();
					}
					currentSymbol = nextSymbol;
				}

				if (nextSymbol->isNonTerminal()) {
					auto leftHandSymbol = r->leftHandSymbol();
					currentFollowSet = followSets_[nextSymbol];
					prevSize = currentFollowSet->size();
					if (currentFollowSet != followSets_[leftHandSymbol]) {
						currentFollowSet->insert(followSets_[leftHandSymbol]->begin(), followSets_[leftHandSymbol]->end());
						CHECK_WEATHER_CURRENTFOLLOWSET_CHANGED();
					}
				}
			}
		}
		if (!changed) 
			break;
		//printMap(followSets_);
		//wcout << L"--------------------" << endl;
	}
	printMap(followSets_);
}

/*
 * confirmCouldProductEmptyStr 用于确定非终结符能否产生空串,这也是一个闭包算法.
 */
void Grammar::confirmCouldProductEmptyStr()
{
	productEmptyStr_.clear();
	for (auto nonTermainal : nonTerminals_) {
		productEmptyStr_[nonTermainal] = false;
	}
	while (true) {
		bool changed = false;
		for (auto r : rules_) {
			symbolPtr leftHandSymbol = r->leftHandSymbol();
			bool prevValue = productEmptyStr_[leftHandSymbol];  /* 记录之前的值 */

#define CHECK_WEATHER_CHANGED()									\
do {															\
			if (prevValue != productEmptyStr_[leftHandSymbol])	\
				changed = true;									\
} while(0)

			if (r->isEpsRule()) {
				productEmptyStr_[leftHandSymbol] = true;
				CHECK_WEATHER_CHANGED();
			}
			else {
				bool shouldBreak = false;
				for (size_t i = 0; i < r->rightHandLength(); ++i) {
					if (productEmptyStr_[r->findNthElem(i)] == false) {
						shouldBreak = true;
						break;
					}
				}
				if (!shouldBreak) {
					productEmptyStr_[leftHandSymbol] = true;
					CHECK_WEATHER_CHANGED();
				}
			}
		}
		if (!changed)  /* 一直要运行到不再发生变化为止 */
			break;
	}
	//printMap(productEmptyStr_);
}

wostream & operator<<(wostream & os, Grammar & g)
{
	for (auto rule : g.rules_) {
		os << *rule;
	}
	return os;
}
