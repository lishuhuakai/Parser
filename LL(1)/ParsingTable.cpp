#include "ParsingTable.h"
#include "Grammar.h"
#include "Symbol.h"

ParsingTable::ParsingTable(Grammar& g) :
	grammar_(g)
{
	/* 接下来构建map */
	for (auto nonTerminal : g.nonTerminals_) {
		auto m = make_shared<map<symbolPtr, rulePtr>>();
		for (auto terminal : g.terminals_) {
			(*m)[terminal] = nullptr;
		}
		table_[nonTerminal] = m;
	}
}


ParsingTable::~ParsingTable()
{
}

map<symbolPtr, rulePtr>& ParsingTable::operator[](symbolPtr & p)
{
	return *table_[p];
}

/*
 * calcFirstSet 用于计算First集合,当然不是单个非终结符的First集合,一般而言,是多个非终结符的First集合.
 */
void ParsingTable::calcFirstSet(rulePtr& rule, set<symbolPtr>& firstSet) {
	bool canDeriveEps = false;
	size_t size = rule->rightHandLength();
	for (size_t i = 0; i < size; ++i) {
		auto currentSymbol = rule->findNthElem(i);
		if (currentSymbol->isNonTerminal()) { /* 非终结符 */
			bool shouldContinue = false;	/* 是否应该继续往下找 */
			for (auto symbol : *grammar_.firstSets_[currentSymbol]) {
				if (!symbol->isEps()) {			/* 这里保证不是非终结符 */
					firstSet.insert(symbol);
				}
				else { /* 如果包含了eps,那么要继续向下寻找 */
					shouldContinue = true;
					if (i == size - 1) { /* 这已经是最后一个非终结符了,表示FIRST(rightHand)可以导出空串 */
						canDeriveEps = true;
					}
				}
			}
			if (!shouldContinue) break;
		}
		else if (!currentSymbol->isEps()) { /* 如果是终结符,并且不是eps */
			firstSet.insert(currentSymbol);
			break;
		}
		else { /* A->ε这种形式 */
			canDeriveEps = true;
			break;
		}
	}
	if (canDeriveEps) { /* 运行到了这里表示,right hand的所有非终结符都可以推出空串 */
		/* 这里的firstSet不能插入eps */
		auto followset = grammar_.followSets_[rule->leftHandSymbol()];
		firstSet.insert(followset->begin(), followset->end());
	}
}
/*
 * buildParsingTable 根据文法,FIRST集和FOLLOW集来构建ParsingTable.
 */
void ParsingTable::buildParsingTable()
{
	grammar_.calcFirstSet();
	grammar_.calcFollowSet();
	for (auto r : grammar_.rules_) {
		set<symbolPtr> firstSet;
		calcFirstSet(r, firstSet); /* 得到所有的first集合 */
		for (auto symbol : firstSet) {
			auto m = table_[r->leftHandSymbol()];
			(*m)[symbol] = r; /* 记录下规则 */
		}
	}
}
