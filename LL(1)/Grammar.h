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
	friend class ParsingTable;
	class iterator
	{
	public:
		friend class Grammar;
		iterator() {}

		/* ������֮����бȽ� */
		bool operator==(const iterator& rhs) const {
			return it_ == rhs.it_;
		}
		bool operator!=(const iterator& rhs) const {
			return it_ != rhs.it_;
		}
		rulePtr& operator*() {
			return *it_;
		}
		iterator& operator++ () {
			iterator tmp = *this;
			it_++;
			return tmp;
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
	symbolPtr startSymbol_;									/* ��ʼ���� */
private:
	set<symbolPtr> nonTerminals_;							/* ���ڼ�¼���еķ��ս�� */
	set<symbolPtr> terminals_;
	map<symbolPtr, symbolSetPtr> followSets_;				/* ���ڼ�¼���е�follow���� */
	map<symbolPtr, symbolSetPtr> firstSets_;				/* ���ڼ�¼���е�first���� */
	map<symbolPtr, bool> productEmptyStr_;					/* ���ڼ�¼���ս���Ƿ��ܹ������մ� */
	vector<rulePtr> rules_;									/* ���ڼ�¼���� */
public:
	shared_ptr<vector<rulePtr>> findRules(symbolPtr& l);	/* ͨ��l,Ѱ����lΪ��ͷ��rule */
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
	void calcFirstSet(); 
	void printMap(map<symbolPtr, symbolSetPtr>& mp);
	void printMap(map<symbolPtr, bool>& mp);
	void calcFollowSet();
	void confirmCouldProductEmptyStr();		/* ����ȷ�����ս���ܷ�����մ� */
private:
};

