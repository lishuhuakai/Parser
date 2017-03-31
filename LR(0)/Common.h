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

template<typename T>
bool insertIntoListIfNotExists(list<T>& l, T& elem) { /* 将元素插入到l中,如果elem在l中不存在的话. */
	for (auto e : l) {
		if (e == elem)
			return false;
	}
	l.push_back(elem);
	return true;
}

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
 * removeElement 从vector中删除对应的元素.
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