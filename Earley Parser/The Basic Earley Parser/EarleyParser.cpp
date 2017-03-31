#include "EarleyParser.h"
#include "Common.h"
#include "Grammar.h"
#include "Rule.h"

/*
 * Item��operator<<����������Ϊ�˵��Է���.
 */
wostream & operator<<(wostream & os, Item & it)
{
	os << *(it.rule->startSymbol()) << " --> ";
	size_t i = 0;
	size_t size = it.rule->length();
	for ( ; i < size; ++i) {
		if (i == it.pos)
			os << L"��";
		os << *(it.rule->findNthElem(i));
	}
	if (i == it.pos)
		os << L"��";
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
	/* �����ǳ�ʼ���Ĺ��� */
	currentPos_ = 0;
	itemsPtr active = make_shared<set<itemPtr>>();
	auto rules = grammar_->findRules(grammar_->startSymbol_);
	for (auto r : *rules) {
		active->insert(makeItem(r, currentPos_, 1));
	}
	auto pred = predictor(active, currentPos_+1); /* �õ�Ԥ�⼯ */
	itemSet_.push_back(combine(active, pred));
	completed_.push_back(make_shared<set<itemPtr>>()); /* ����һ���յ�completed1 */
	while (true) {
		if (currentPos_ == str.size()) {
			/* ʲôʱ������ʧ����?�� */
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
 * combine ��active��pred�������Ϻϲ�,����һ��itemset.
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
 * success �����ж��Ƿ�����ɹ�.
 */
bool EarleyParser::success()
{
	for (auto item : *completed_.back()) { /* ֻ��Ҫ�鿴���һ��completed���ϼ��� */
		if (*(item->rule->startSymbol()) == *(grammar_->startSymbol_) && item->at == 1) { /* �ҵ��˿�ʼ���� */
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

EarleyParser::itemsPtr EarleyParser::predictor(itemsPtr& active, int at) { /* �Թ������Ԥ�� */
	itemsPtr pred = make_shared<set<itemPtr>>();
	list<symbolPtr> newSymbol; /* ���뾡����Чһ��,��Ҫ�������ͱ������еĹ��� */
	for (auto r : *active) {
		symbolPtr s = r->rule->findNthElem(r->pos); /* �ҵ���Ӧλ�õķ��� */
		//wcout << *r;
		if (s->isNonTerminal())
			insertIntoListIfNotExists(newSymbol, s);
	}
	/* ���黹û�����,����pred��������ķ��ս��,ҲҪ��չ���� */
	for (auto it = newSymbol.begin(); it != newSymbol.end(); ++it) {
		auto rules = grammar_->findRules(*it);
		for (auto rule : *rules) { /* ������ѹ��pred */
			//wcout << *rule;
			symbolPtr s = rule->findNthElem(0);
			if (s->isNonTerminal())
				insertIntoListIfNotExists(newSymbol, s); /* newSymbol���ڱ仯��,����list�ĵ��������ᷢ���仯 */
			pred->insert(makeItem(rule, 0, at));
		}
	}
	wcout << "pred:" << endl;
	printItems(pred);
	return pred;
}


struct CompletedItem {
	symbolPtr s;	/* ���ڱ�Ƿ��� */
	int at;			/* ��ʼ��λ�� */
	friend bool operator==(CompletedItem& l, CompletedItem& r) {
		return *(l.s) == *(r.s) && l.at == r.at;
	}
	CompletedItem(symbolPtr& s, int at) :
		s(s), at(at)
	{}
};
typedef shared_ptr<CompletedItem> completedItemPtr;
/*
 * distributor ���itemset,ʵ�ַַ��Ĺ���.
 */
void EarleyParser::distributor(itemsPtr & itemset, symbolPtr& ch, itemsPtr& completed, itemsPtr& active)
{
	list<completedItemPtr> symbols;
	symbols.push_back(make_shared<CompletedItem>(ch, currentPos_));
	itemsPtr currentSet;
	for (auto s : symbols) {
		/* ���ÿһ��Item,�ҵ�ƥ���� */
		currentSet = itemSet_[s->at];
		for (auto item : *currentSet) {
			symbolPtr symbol = item->rule->findNthElem(item->pos);
			if (*symbol == *(s->s)) { /* ��������ƥ����ַ� */
				auto newItem = copyItem(item);
				newItem->pos++;
				if (newItem->pos == newItem->rule->length()) { /* �����������,���Թ�Լ�� */
					completed->insert(newItem);
					/* ������Ҫ����һ����,���newItem��at����currentPos_,��ôҪ���⴦�� */
					int at = item->at - 1;
					if (at >= 0)
						insertIntoListIfNotExists(symbols, make_shared<CompletedItem>(item->rule->startSymbol(), at));
				}
				else { /* ����Ļ�,Ҫ����active������ */
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

