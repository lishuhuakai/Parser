#include "Grammar.h"
#include "Rule.h"
#include "Symbol.h"


Grammar::Grammar(symbolPtr& s) :
	start_(s)
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
		if (rule->originEqualTo(l)) {
			result->push_back(rule);
		}
	}
	return result;
}

/*
 * getAllSymbols ���ڻ�ȡrule�����еķ���.
 */
void Grammar::getAllSymbols() {
	for (auto r : rules_) {
		symbols_.insert(r->origin());
		for (size_t i = 0; i < r->expansionLength(); ++i)
			symbols_.insert(r->findNthElem(i));
	}
}

/*
 * updateSet ����s������t,���t�ı���,��ô����true,���򷵻�false
 */
bool Grammar::updateSet(set<symbolPtr>& t, set<symbolPtr>& s) {
	size_t len = t.size();
	t.insert(s.begin(), s.end());
	return t.size() != len;
}

bool Grammar::updateSet(set<symbolPtr>& t, symbolPtr& s) {
	size_t len = t.size();
	t.insert(s);
	return t.size() != len;
}

/*
 * calculateSets ���ڼ���FIRST����FOLLOW��
 */
void Grammar::calculateSets()
{
	/* ��һ��,������еķ��� */
	getAllSymbols();
	/* ����������ʼ���Ĺ��� */
	for (auto sym : symbols_) {
		first_[sym] = make_shared<symbolSet>();
		follow_[sym] = make_shared<symbolSet>();
		if (sym->isTerminal())
			first_[sym]->insert(sym);
	}
	bool changed = true;
	while (changed) {
		changed = false;
		for (auto r : rules_) {
			/* �������r��expansion�����ķ���ȫ�������Ƶ���epsilon,��ôr��orginҲ�����Ƶ���epsilon */
			if (isSubSet(set<symbolPtr>(r->expansion().begin(), r->expansion().end()), nullable_)) {
				if (updateSet(nullable_, r->origin())) changed = true;
			}

			size_t len = r->expansionLength();
			for (size_t i = 0; i < len; ++i) {
				auto subExpansion = r->subExpansionSymbols(0, i);
				auto sym = r->findNthElem(i);
				auto origin = r->origin();
				/* ���r��expansion��0��i(������i)��Ԫ�ض������Ƶ���epsilon, ��ôfirst(r.origin)����first(r.findNthElem(i)) */
				if (isSubSet(*subExpansion, nullable_)) {
					if (updateSet(*first_[origin], *first_[sym]))
						changed = true;
				}
				 /* ��i��Ԫ�ز������һ��Ԫ��,����expansion�ӵ�i+1��Ԫ��֮���Ԫ��ȫ��nullable 
				  * ��ôfollow[sym]����follow[r.origin].
				  * �����i��Ԫ�������һ��Ԫ��,ͬ����follow[sym]����follow[sym].
				  */
				if ((i == (len - 1)) || isSubSet(*r->subExpansionSymbols(i + 1, len - i - 1), nullable_)) { 
						if (updateSet(*follow_[sym], *follow_[origin])) changed = true;
				}

				/* ����ӵ�i+1����j(������j)λ�õ�Ԫ�ض���nullable��,��ôFollow[sym]����first[r->findNthElem(j)] */
				for (size_t j = i + 1; j < len; j++) {
					/* ��j��Ԫ�ر��������expansion */
					size_t len = j - i - 1;
					subExpansion = r->subExpansionSymbols(i + 1, len);
					if (isSubSet(*subExpansion, nullable_)) {
						if (updateSet(*follow_[sym], *first_[r->findNthElem(j)])) changed = true;
					}
				}
			}
		}
	}
	//wcout << L"================================================" << endl;
	//printSets();
}

wostream & operator<<(wostream & os, Grammar & g)
{
	for (auto rule : g.rules_) {
		os << *rule;
	}
	return os;
}

void Grammar::printSets() {
	wcout << L">>>>>>>>>>first<<<<<<<<<<" << endl;
	for (auto pr : first_) {
		wcout << *(pr.first) << L":" << endl;
		for (auto sym : *pr.second) {
			wcout << *sym << L" ";
		}
		wcout << endl;
	}

	wcout << L">>>>>>>>>>follow<<<<<<<<<<" << endl;
	for (auto pr : follow_) {
		wcout << *(pr.first) << L":" << endl;
		for (auto sym : *pr.second) {
			wcout << *sym << L" ";
		}
		wcout << endl;
	}

	wcout << L">>>>>>>>>>nullable<<<<<<<<<<" << endl;
	for (auto sym : nullable_) {
		wcout << *sym << L" ";
	}
	wcout << endl;
}
