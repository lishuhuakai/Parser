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

/*
 * ���Ҫ��ʼ����Ź���Ļ�,Parser�����ø��Ӹ���.
 */

bool EarleyParser::startParse(vector<symbolPtr>& str) {
	/* �����ǳ�ʼ���Ĺ��� */
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
			active->push_back(makeItem(r, currentPos_, 1)); /* �տ�ʼ��ʱ�򽫴�s�����Ĺ���ȫ��ѹ��ȥ */
	}
	closure(active, pred, completed, false); /* ���ñհ��㷨,ֱ�����߲��ٷ����仯,false��ʾδ�����ù�distributor���� */
	wcout << L"itemset" << endl;
	auto itemset = combine(active, pred);
	printItems(itemset);
	itemSet_.push_back(itemset);
	completed_.push_back(completed); /* ����һ���յ�completed1 */
	while (true) {
		if (currentPos_ == str.size()) {
			/* ʲôʱ������ʧ����?�� */
			return success();
		}
		completed = make_shared<vector<itemPtr>>();
		active = make_shared<vector<itemPtr>>();
		pred = make_shared<vector<itemPtr>>();
		/*
		 * distributor�����ı�����ʲô,��ɨ��ǰһ��itemset,��ƥ���item advanced 1λ,
		 * ����active����completed����.
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
 * combine ��active��pred�������Ϻϲ�,����һ��itemset.
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


/*
 * completer���ڴ���������completed��Ԫ��,Ҳ��������Ҫ����ĦŹ���.
 * 
 */
size_t EarleyParser::completer(itemsPtr & completed, itemsPtr & active, size_t& completedPos)
{
	itemsPtr currentItemSet;		/* ���ڼ�¼��ǰ��set */
	vector<itemPtr> newCompletedItems;
	vector<itemPtr> newActiveItems;
	/*
	* ֻ�����������item.
	*/
	for (; completedPos < completed->size(); ++completedPos) {
		auto item = (*completed)[completedPos]; /* ����µ�item */
		/* ������Ҫ˵��һ��,�¼����item������������ʽ��:
		 *  A -> �� @currentPos, ����currentPos�ǵ�ǰ���ڴ����λ��.
		 * ��������������ʽ��:
		 * B -> XXX�� @pos ,����X�ȿ��Դ����ս��Ҳ���Դ�����ս��,posС��currentPos
		 * ���һ���������������,��ôҪ��itemset(pos)ȥѰ�ҡ�B��ʽ��δ��ɵ�item.
		 */
		if ((item->at - 1) < currentPos_) /* atһ����±��1 */
			currentItemSet = itemSet_[item->at - 1];
		else
			currentItemSet = active;

		for (auto it : *currentItemSet) { /* ����currentSet,��ʼѰ�� */
			symbolPtr symbol = it->rule->findNthElem(it->pos);
			if (*symbol == *(item->rule->startSymbol())) { /* ���ƥ��Ļ�Ҫ����µ�item��active�� */
				auto newItem = copyItem(it);
				newItem->pos++;
				/* �����ǰ�ƶ�һ��,������end,���Թ�Լ��,ҲҪ����ѹ��completed�� */
				if (newItem->pos == newItem->rule->length()) {
					/* ��������������Ż�,��Ϊ�����Ǵ�������ӵ�completed��,���Ǵ���֮����Ȼ��һ��������completed�� */
					newCompletedItems.push_back(newItem);  /* ��ֹ������ʧЧ */
				}
				else
					newActiveItems.push_back(newItem); /* ��ֹ������ʧЧ */
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
 * ����distributed���ڱ�ʾ�Ƿ���ù�distributor����.
 */
void EarleyParser::closure(itemsPtr &active, itemsPtr &pred, itemsPtr& completed, bool distributed)
{
	size_t activePos = 0;
	size_t completedPos = 0;
	/*
	 * ������ù�distributor����,˵��completed�����Ԫ�ض��Ѿ����������,����completedPosҪ����Ϊcompleted->size()
	 * ����Ļ�,����û�д����,Ҫ��ͷ��ʼ����.
	 */
	if (distributed)
		completedPos = completed->size();
	while (true) { /* �հ��㷨,һֱ���е����߲��ٷ����仯Ϊֹ */
		/* predictor���ܻᵼ��completed��С�����仯 */
		if (predictor(active, pred, completed, activePos, currentPos_ + 1) != completedPos) {
			/* completer���ܻᵼ��completed��active�����仯 */
			if ((completer(completed, active, completedPos) == activePos) && (completedPos == completed->size()))
				break;
		}
		else break;
	}
	wcout << L"completed:" << endl;
	printItems(completed);
}


/*
 * predictor�Ĺ�����ʼ�����һЩ������.
 */
size_t EarleyParser::predictor(itemsPtr& active, itemsPtr& pred, itemsPtr& completed, 
	size_t& activePos, int at) { /* �Թ������Ԥ�� */

	list<symbolPtr> newSymbol; /* ���뾡����Чһ��,��Ҫ�������ͱ������еĹ��� */
	/* ��active�в����ڦ� */ 
	for (; activePos < active->size(); ++activePos) {
		itemPtr item = (*active)[activePos];
		symbolPtr s = item->rule->findNthElem(item->pos); /* �ҵ���Ӧλ�õķ��� */
		//wcout << *(item->rule);
		if (s->isNonTerminal()) /* �����ս����ȫ������newSymbol�� */
			insertIntoListIfNotExists(newSymbol, s);
	}
	/* ���黹û�����,����pred��������ķ��ս��,ҲҪ��չ���� */
	for (auto it = newSymbol.begin(); it != newSymbol.end(); ++it) {
		auto rules = grammar_->findRules(*it); /* ���ݷ��ս��Ѱ�ҹ��� */
		for (auto rule : *rules) { /* ������ѹ��pred */
			if (rule->isEpsRule()) { /* �����һ��EPS����Ļ�,Ҫ����������ѹ��completed���� */
				insertIntoVectorIfNotExists(*completed, makeItem(rule, 0, at));
				//completed->push_back();
			}
			else
			{
				symbolPtr s = rule->findNthElem(0);
				if (s->isNonTerminal())
					insertIntoListIfNotExists(newSymbol, s); /* newSymbol���ڱ仯��,����list�ĵ��������ᷢ���仯 */
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
	/*
	* distrubutor��completer���������ڹ����Ϻ����,�ǲ��ǿ���ʵ�����������ĺϲ���?
	* ĳЩ���ܿ��Ժϲ�,���ǲ�����,��Ϊ������Щ����,��ʱ��������.
	*/
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
					completed->push_back(newItem);
					/* ������Ҫ����һ����,���newItem��at����currentPos_,��ôҪ���⴦�� */
					int at = item->at - 1;
					if (at >= 0)
						insertIntoListIfNotExists(symbols, make_shared<CompletedItem>(item->rule->startSymbol(), at));
				}
				else { /* ����Ļ�,Ҫ����active������ */
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
