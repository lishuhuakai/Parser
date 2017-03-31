#include "Symbol.h"


symbolPtr Symbol::eps = make_shared<Symbol>(Symbol::Terminal, L"");
symbolPtr Symbol::eof = make_shared<Symbol>(Symbol::EndOfFile, L"");

Symbol::Symbol(symbolType t, const wstring& str) :
	type_(t), content_(str)
{
}

Symbol::~Symbol()
{
}

/*
 * uniqueMark 每个符号都有对应的唯一的符号.
 */
wstring Symbol::uniqueMark()
{
	if (type_ == EndOfFile)
		return L"#";
	else if (isEOF())
		return L"Eps";
	else
		return content_;
}

wostream& operator<<(wostream& os, Symbol& s) { /* 输出符号 */
	if (s.isEps())
		os << L"eps";
	else if (s.isEOF())
		os << L"#";
	else
		os << s.content_.c_str();
	return os;
}
