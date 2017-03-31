#include "Symbol.h"


Symbol::Symbol(symbolType t, const wstring& str) :
	type_(t), content_(str)
{
}

Symbol::~Symbol()
{
}
