#pragma once
#include "Common.h"

/*
 * Symbol��������ʾ�ս���ͷ��ս��.
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
	bool isEps() {
		return content_.size() == 0 && type_ == Terminal;
	}
	friend wostream& operator<<(wostream&, Symbol&);
	wstring uniqueMark() {
		return content_;
	}
	bool operator==(Symbol& r) {
		return this->content_ == r.content_;
	}
public:
	symbolType type_;	/* �������ַ��ŵ����� */
	wstring content_;	/* ���ڼ�¼���ŵ����� */
};


