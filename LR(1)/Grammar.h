#pragma once
#include "Common.h"
#include "Rule.h"
#include "Symbol.h"
class ParsingTable;

class Grammar
{
	typedef set<symbolPtr> symbolSet;
	typedef shared_ptr<symbolSet> symbolSetPtr;
public:
	class iterator
	{
	public:
		friend class Grammar;
		iterator() {}

		/* 迭代器之间进行比较 */
		bool operator==(const iterator& rhs) const {
			return it_ == rhs.it_;
		}
		bool operator!=(const iterator& rhs) const {
			return it_ != rhs.it_;
		}
		rulePtr& operator*() {
			/* 这里有必要说明一下,it_是迭代器类型, *it_是rulePtr类型, **it_是Rule类型 */
			return *it_;
		}
		iterator& operator++ () {
			it_++;
			return *this;
		}
		iterator& operator--() {
			--it_;
			return *this;
		}
	private:
		vector<rulePtr>::iterator it_;
		iterator(vector<rulePtr>::iterator& it) :
			it_(it)
		{}
	};
public:
	Grammar(symbolPtr&);
	~Grammar();
public:
	symbolPtr startSymbol_;									/* 开始符号 */
private:
	set<symbolPtr> nonTerminals_;							/* 用于记录所有的非终结符 */
	set<symbolPtr> terminals_;
	map<symbolPtr, symbolSetPtr> followSets_;				/* 用于记录所有的follow集合 */
	map<symbolPtr, symbolSetPtr> firstSets_;				/* 用于记录所有的first集合 */
	map<symbolPtr, bool> productEmptyStr_;					/* 用于记录非终结符是否能够产生空串 */
	vector<rulePtr> rules_;									/* 用于记录规则 */
public:
	shared_ptr<vector<rulePtr>> findRules(symbolPtr& l);	/* 通过l,寻找以l为开头的rule */
	friend wostream& operator<<(wostream& os, Grammar& g);
	void appendRule(rulePtr& r) {
		nonTerminals_.insert(r->leftHandSymbol());
		auto terminals = r->getTerminals();
		//terminals.insert(make_shared<Symbol>(Symbol::NonTerminal, L" "));
		terminals_.insert(terminals.begin(), terminals.end());
		rules_.push_back(r);
	}
	iterator begin() {
		return iterator(rules_.begin());
	}
	iterator end() {
		return iterator(rules_.end());
	}
	vector<rulePtr> getAllRules() {  /* 虽然我也不想拷贝一份,但是没有办法,我不能将内部的数据暴露出去. */
		return rules_;
	}
	void calcFirstSet(); 
	void printMap(map<symbolPtr, symbolSetPtr>& mp);
	void printMap(map<symbolPtr, bool>& mp);
	void calcFollowSet();
	void confirmCouldProductEmptyStr();		/* 用于确定非终结符能否产生空串 */
private:
};

