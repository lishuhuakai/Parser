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
 * findRules ����Ѱ�ҷ��ս��l���Ƶ�����.
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
 * calcFirstSet �������з��ս���ŵ�FIRST��,����һ���հ��㷨,�����������ܻ���һ���. 
 */
void Grammar::calcFirstSet()
{
	firstSets_.clear();
	/* ��һ��Ҫ������,�����е�first��������Ϊ��. */
	for (auto nt : nonTerminals_) {
		firstSets_[nt] = make_shared<symbolSet>();
	}
	/* ��������ʼ���� */
	while (true) {
		bool changed = false;
		for (auto r : rules_) {
			symbolSetPtr firstSet = firstSets_[r->leftHandSymbol()];
			size_t size = firstSet->size();
			/* ÿ�β�����ϵ�ʱ����Ҫ����Ƿ�ı���,���Ըɴཫ�������д�ɺ� */
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
				symbolPtr rightHandFirstElemt = r->findNthElem(0);	/* �õ��Ҳ�ĵ�һ��Ԫ�� */
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
							/* �����û����������ս����FIRST���ҵ���,��ôҪֹͣ */
							break;
						}
					}
				}
				else if (rightHandFirstElemt->isTerminal()) {		/* ���ս��ֱ�Ӳ��� */
					firstSet->insert(rightHandFirstElemt);
					DETECT_WEATHER_CHANGED(size);
				}
			}
		}
		//printFirstSet();
		//wcout << L"--------------------" << endl;
		if (!changed)  /* ��Ҫһֱ���������ٷ����仯 */
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
 * calcFollowSet �������з��ս���ŵ�FOLLOW��,ͬ��Ҳ��һ���հ��㷨.
 */
void Grammar::calcFollowSet()
{
	followSets_.clear();
	/* ��һ��Ҫ������,�����е�first��������Ϊ��. */
	for (auto nt : nonTerminals_) {
		followSets_[nt] = make_shared<symbolSet>();
	}
	confirmCouldProductEmptyStr();		/* ȷ����һЩ���ս�����Բ����մ�,ֻ����һ�ξ�����. */
	if (firstSets_.size() == 0)
		calcFirstSet();
	//printMap(firstSets_);
	//wcout << L"______firstSet--------" << endl;
	followSets_[startSymbol_]->insert(Symbol::eof);  /* ��ʼ���ŵ�follow����һ��������ֹ���� */
	while (true) {
		bool changed = false;
		for (auto r : rules_) {
			size_t size = r->rightHandLength();
			size_t prevSize;  /* ���ڼ�¼��ǰ���ϵĴ�С */
			symbolSetPtr currentFollowSet;

#define CHECK_WEATHER_CURRENTFOLLOWSET_CHANGED()		\
do {													\
			if (prevSize != currentFollowSet->size())	\
				changed = true;							\
}while(0)

			if (!r->isEpsRule() && size == 1) { // A -> x ������ʽ
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
					if (currentSymbol->isTerminal()) { /* ��������ս�� */
						currentSymbol = nextSymbol;
						continue;
					}
					currentFollowSet = followSets_[currentSymbol];
					prevSize = currentFollowSet->size();

					if (currentSymbol->isNonTerminal() && nextSymbol->isNonTerminal()) {
						for (auto symbol : *firstSets_[nextSymbol]) {
							if (!symbol->isEps())  /* ����eps,����ȫ������currentFollowSet */
								currentFollowSet->insert(symbol);
						}
						if (productEmptyStr_[nextSymbol]) {	/* �����һ�����ս�����Բ����մ�,��ôҪ��nextSymbol��follow������currentSymbol��follow�� */
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
 * confirmCouldProductEmptyStr ����ȷ�����ս���ܷ�����մ�,��Ҳ��һ���հ��㷨.
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
			bool prevValue = productEmptyStr_[leftHandSymbol];  /* ��¼֮ǰ��ֵ */

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
		if (!changed)  /* һֱҪ���е����ٷ����仯Ϊֹ */
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
