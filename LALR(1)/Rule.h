#pragma once
#include "Common.h"
#include "Symbol.h"
#include <functional>

/*
 * Rule����Ҫ���ڼ�¼�﷨,����˵�ķ�.
 */
class Rule
{
public:
	Rule(symbolPtr&);
	~Rule();
public:
	bool isEpsRule() {						/* �����ж����������Ƿ��ǦŹ��� */
		return expansion_.size() == 0;
	}

	bool originEqualTo(symbolPtr& s) {		/* �ж�rule������Ƿ���s */
		return s == origin_;
	}

	symbolPtr findNthElem(size_t n) {		/* Ѱ��rule�Ƶ�ʽ�Ҳ�ĵ�N��Ԫ�� */
		assert(n < expansion_.size());
		return expansion_[n];
	}
	
	vector<symbolPtr>& expansion() {
		return expansion_;
	}

	vector<symbolPtr> subExpansion(size_t from, size_t len) {
		auto start = expansion_.begin();
		return vector<symbolPtr>(start + from, start + from + len);
	}

	shared_ptr<set<symbolPtr>> subExpansionSymbols(size_t from, size_t len) {
		auto start = expansion_.begin();
		return make_shared<set<symbolPtr>>(start + from, start + from + len);
	}

	size_t expansionLength() {
		return expansion_.size();
	}

	symbolPtr origin() {		/* ��ȡ�������ķ��ս�� */
		return origin_;
	}

	Rule(const Rule& r);

	void expansionAppend(symbolPtr& s) {
		expansion_.push_back(s);
	}

public:
	friend wostream& operator<<(wostream& os, Rule& r);
	bool operator==(Rule& r);

private:
	symbolPtr origin_;					/* ��߲��� */
	vector<symbolPtr> expansion_;		/* չ������ */
};


/*
* Item ���ڼ�¼һЩ״̬,˵ʵ��,Item�Ѿ��ǳ�������һ��ָ��,��ֻ�Ǽ�¼��rule�ĵ�ַ,Ȼ�󸽴���һ��pos,���Ը��ƵĿ���Ҳ������.
*/
class Item
{
public:
	Item(rulePtr&, int);
	~Item();

private:
	rulePtr rule_;		/* ���ڼ�¼�Ƶ����� */
	int pos_;			/* ���ڼ�¼Item�����Ѿ���������rule��right hand����һ����������. */

public:
	friend wostream& operator<<(wostream& os, Item& it);
	
	bool operator==(const Item& r) const {		/* ���ڱȽ�����Item�Ƿ���� */
		return (rule_ == r.rule_) && (pos_ == r.pos_);
	}

	int getPos() {
		return pos_;
	}

	/* ÿ��item����һ��hashֵ 
	 * ���item��ȵĻ�,���ǵ�hashֵҲ���.
	 */
	size_t hash() {
		size_t h1 = std::hash<shared_ptr<Rule>>{}(rule_);
		size_t h2 = std::hash<int>{}(pos_);
		return h1 ^ (h2 << 1);
	}

	rulePtr getRule() {
		return rule_;
	}

	/*
	 * isSatisfied �����ж�pos�Ƿ���rule�����һ��λ��,Ҳ�����ж��ܷ��Լ��.
	 */
	bool isSatisfied() {
		return pos_ == rule_->expansionLength();
	}

	/*
	 * next��ȡrule_��Expansion�е�pos_������,����s��,�ɹ�����true,���򷵻�false
	 */
	bool next(symbolPtr& s) {
		if (!isSatisfied()) {
			s = rule_->findNthElem(pos_);
			return true;
		}
		else
			return false;
	}

	/*
	 * advance��pos_ǰ��һ��,�޷�ǰ��ʱ,����false,���򷵻�true
	 */
	bool advance() {
		if (!isSatisfied()) {
			pos_++;
			return true;
		}
		else
			return false;
	}

	bool operator<(const Item& r) const {	 /* ������������ò���,���ǻ��Ƿ��������. */
		if (rule_ != r.rule_)
			return rule_ < r.rule_;
		else 
			return pos_ < r.pos_;
	}

	Item(const Item& r) {		/* �������캯�� */
		this->rule_ = r.rule_;
		this->pos_ = r.pos_;
	}

};
typedef shared_ptr<Item> itemPtr;

