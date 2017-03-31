#pragma once
#include "Common.h"

/*
 * Symbol��������ʾ�ս���ͷ��ս��.
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
	bool isEOF() { /* �ս�� */
		return type_ == EndOfFile;
	}
	friend wostream& operator<<(wostream&, Symbol&);
	wstring uniqueMark();
	bool operator==(Symbol& r) {
		return this->content_ == r.content_;
	}
public:
	symbolType type_;	/* �������ַ��ŵ����� */
	wstring content_;	/* ���ڼ�¼���ŵ����� */
};


