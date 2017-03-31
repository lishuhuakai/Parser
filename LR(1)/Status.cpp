#include "Status.h"

wostream & operator<<(wostream & os, Status & s)
{
	os << L"label : " << s.label << endl;
	switch (s.kind)
	{
	case Status::kStation:
		os << L"¡ñ" << *s.nonTerminal << *s.lookAheadSymbol;
		break;
	case Status::kStatus:
		for (auto it : s.items) {
			os << it << endl;
		}
		break;
	default:
		break;
	}
	return os;
}
