#pragma once
#include "Common.h"

/*
 * Symbol类用来表示终结符和非终结符.
 */
class Symbol
{
public:
	enum symbolType {
		Terminal, NonTerminal, EndOfFile
	};
public:
	Symbol(symbolType,const wstring&);
	~Symbol();
	static symbolPtr eps;
	static symbolPtr eof;
public:
	bool isTerminal() {
		return type_ == Terminal;
	}
	bool isNonTerminal() {
		return type_ == NonTerminal;
	}
	bool isEps() {
		return content_.size() == 0 && type_ == Terminal;
	}
	bool isEOF() { /* 终结符 */
		return type_ == EndOfFile;
	}
	friend wostream& operator<<(wostream&, Symbol&);
	wstring uniqueMark();
	bool operator==(Symbol& r) {
		return this->content_ == r.content_;
	}
public:
	symbolType type_;	/* 用来区分符号的类型 */
	wstring content_;	/* 用于记录符号的内容 */
};


