#pragma once
#include "Common.h"
#include "Rule.h"
#include "Symbol.h"

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

		/* ������֮����бȽ� */
		bool operator==(const iterator& rhs) const {
			return it_ == rhs.it_;
		}
		bool operator!=(const iterator& rhs) const {
			return it_ != rhs.it_;
		}
		rulePtr& operator*() {
			/* �����б�Ҫ˵��һ��,it_�ǵ���������, *it_��rulePtr����, **it_��Rule���� */
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
	symbolPtr start_;								/* ��ʼ���� */
private:
	set<symbolPtr> symbols_;							/* ���ڼ�¼���еķ���,�����ս���ͷ��ս�� */
	map<symbolPtr, symbolSetPtr> follow_;				/* ���ڼ�¼���е�follow���� */
	map<symbolPtr, symbolSetPtr> first_;				/* ���ڼ�¼���е�first���� */
	set<symbolPtr> nullable_;							/* ���ڼ�¼���ս���Ƿ��ܹ������մ� */
	vector<rulePtr> rules_;								/* ���ڼ�¼���� */
public:
	shared_ptr<vector<rulePtr>> findRules(symbolPtr& l);	/* ͨ��l,Ѱ����lΪ��ͷ��rule */
	friend wostream& operator<< (wostream& os, Grammar& g);
	void appendRule(rulePtr& r) {
		rules_.push_back(r);
	}
	iterator begin() {
		return iterator(rules_.begin());
	}
	iterator end() {
		return iterator(rules_.end());
	}

	vector<rulePtr> getAllRules() {  /* ��Ȼ��Ҳ���뿽��һ��,����û�а취,�Ҳ��ܽ��ڲ������ݱ�¶��ȥ. */
		return rules_;
	}
	/* startRule���������ķ�����ʼ���� */
	rulePtr startRule() {
		return (*findRules(start_))[0];
	}
	void calculateSets();
	void printSets();
	set<symbolPtr> follow(symbolPtr& s) {
		return *follow_[s];
	}
	set<symbolPtr> first(symbolPtr& s) {
		return *first_[s];
	}
private:
	void getAllSymbols();
	static bool updateSet(set<symbolPtr>& t, set<symbolPtr>& s);
	static bool updateSet(set<symbolPtr>& t, symbolPtr& s);
};

