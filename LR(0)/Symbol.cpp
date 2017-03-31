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
