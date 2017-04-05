#pragma once
#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>
#include <queue>
#include <assert.h>
#include <set>
#include <map>
#include <list>
using namespace std;

class Rule;
class Symbol;
class Grammar;
typedef shared_ptr<Symbol> symbolPtr;
typedef shared_ptr<Rule> rulePtr;
typedef shared_ptr<Grammar> grammarPtr;

///////////////////////////////////////////////////
//   about list
///////////////////////////////////////////////////

template<typename T>
bool insertIntoListIfNotExists(list<T>& l, T& elem) { /* ��Ԫ�ز��뵽l��,���elem��l�в����ڵĻ�. */
	for (auto e : l) {
		if (e == elem)
			return false;
	}
	l.push_back(elem);
	return true;
}

///////////////////////////////////////////////////
//   about vector
///////////////////////////////////////////////////

template<typename T>
bool insertIntoVectorIfNotExists(vector<T>& v, T& elem) {
	for (auto e : v) {
		if (*e == *elem)
			return false;
	}
	v.push_back(elem);
	return true;
}

template<typename T>
bool findElement(const vector<T>& vec, T v) {
	for (auto i : vec) {
		if (i == v)
			return true;
	}
	return false;
}


/*
 * removeElement ��vector��ɾ����Ӧ��Ԫ��.
 */
template<typename T>
void removeElement(vector<T>& vec, const T & element)
{
	for (vector<T>::iterator it = vec.begin(); it != vec.end(); ++it) {
		if (*it == element) {
			vec.erase(it);
			return;
		}
	}
}

///////////////////////////////////////////////////
//   about set
///////////////////////////////////////////////////

/*
 * isSubSet �����ж�l�Ƿ���r���Ӽ�,�㷨���Ӷ�O(N)
 */
template<typename T>
bool isSubSet(set<T>& l, set<T>& r) {
	if (r.size() < l.size()) return false;
	auto e1 = r.begin(), e2 = l.begin();
	for ( ; e2 != l.end() && e1 != r.end(); ) {
		if (*e1 == *e2) {
			e1++, e2++;
		}
		else
			e1++;
	}
	if (e2 == l.end())
		return true;
	return false;
}

/*
 * �ķ��Ĵ��� 
 */
class GrammarError {
public:
	int line_;   
	int pos_;
	wstring message_;
};