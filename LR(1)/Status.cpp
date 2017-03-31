#include "Status.h"

wostream & operator<<(wostream& os, Status& s)
{
	os << L"label : " << s.label << endl;
	if (s.kind == Status::kStation)
		os << L"* [¡ñ" << *s.nonTerminal << L"	" << *s.lookAheadSymbol << L"]" << endl;
	if (s.items.size() != 0) {
		for (auto it : s.items) {
			os << it << L"	" << *it.getLookAhead() << endl;
		}
	}
	os << endl;
	return os;
}

wostream & operator<<(wostream & os, Edge & e)
{

	wcout << L"label: " << e.label << endl;
	wcout << e.from->label << L"==>" << e.to->label;
	if (e.isEps())
		wcout << L"	eps" << endl;
	else
		wcout << L"	" << *(e.matchContent[0]) << endl;
	return os;
}
