#pragma once
#include "Common.h"

/*
 * Symbol类用来表示终结符和非终结符.
 */
class Symbol
{
public:
	enum symbolType {
		Terminal, NonTerminal
	};
public:
	Symbol(symbolType,const wstring&);
	~Symbol();
public:
	bool isTerminal() {
		return type_ == Terminal;
	}
	bool isNonTerminal() {
		return type_ == NonTerminal;
	}
	friend wostream& operator<<(wostream& os, Symbol& s) { /* 输出符号 */
		os << s.content_.c_str();
		return os;
	}
	bool operator==(Symbol& r) {
		return this->content_ == r.content_;
	}
public:
	symbolType type_;	/* 用来区分符号的类型 */
	wstring content_;	/* 用于记录符号的内容 */
};


