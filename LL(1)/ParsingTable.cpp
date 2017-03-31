#include "ParsingTable.h"
#include "Grammar.h"
#include "Symbol.h"

ParsingTable::ParsingTable(Grammar& g) :
	grammar_(g)
{
	/* ����������map */
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
 * calcFirstSet ���ڼ���First����,��Ȼ���ǵ������ս����First����,һ�����,�Ƕ�����ս����First����.
 */
void ParsingTable::calcFirstSet(rulePtr& rule, set<symbolPtr>& firstSet) {
	bool canDeriveEps = false;
	size_t size = rule->rightHandLength();
	for (size_t i = 0; i < size; ++i) {
		auto currentSymbol = rule->findNthElem(i);
		if (currentSymbol->isNonTerminal()) { /* ���ս�� */
			bool shouldContinue = false;	/* �Ƿ�Ӧ�ü��������� */
			for (auto symbol : *grammar_.firstSets_[currentSymbol]) {
				if (!symbol->isEps()) {			/* ���ﱣ֤���Ƿ��ս�� */
					firstSet.insert(symbol);
				}
				else { /* ���������eps,��ôҪ��������Ѱ�� */
					shouldContinue = true;
					if (i == size - 1) { /* ���Ѿ������һ�����ս����,��ʾFIRST(rightHand)���Ե����մ� */
						canDeriveEps = true;
					}
				}
			}
			if (!shouldContinue) break;
		}
		else if (!currentSymbol->isEps()) { /* ������ս��,���Ҳ���eps */
			firstSet.insert(currentSymbol);
			break;
		}
		else { /* A->��������ʽ */
			canDeriveEps = true;
			break;
		}
	}
	if (canDeriveEps) { /* ���е��������ʾ,right hand�����з��ս���������Ƴ��մ� */
		/* �����firstSet���ܲ���eps */
		auto followset = grammar_.followSets_[rule->leftHandSymbol()];
		firstSet.insert(followset->begin(), followset->end());
	}
}
/*
 * buildParsingTable �����ķ�,FIRST����FOLLOW��������ParsingTable.
 */
void ParsingTable::buildParsingTable()
{
	grammar_.calcFirstSet();
	grammar_.calcFollowSet();
	for (auto r : grammar_.rules_) {
		set<symbolPtr> firstSet;
		calcFirstSet(r, firstSet); /* �õ����е�first���� */
		for (auto symbol : firstSet) {
			auto m = table_[r->leftHandSymbol()];
			(*m)[symbol] = r; /* ��¼�¹��� */
		}
	}
}
