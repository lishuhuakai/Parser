#ifndef DFA_H
#define DFA_H
#include "common.h"
#include "Nfa.h"
#include "ActionGoto.h"

class TwoDimArray;

class Dfa
{
public:
	Dfa(Nfa& n);
	~Dfa();
public:
	shared_ptr<ActionGoto> buildDFA();
private:
	Nfa& nfa_;
	bool builted;
private:
	void getAllSymbols(shared_ptr<set<statusPtr>>&, set<symbolPtr>&);
	int assignNewStatusLabel();
};

#endif // !DFA_H
