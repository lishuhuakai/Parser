#include "EpsNfa.h"
#include "Grammar.h"
#include <functional>

EpsNfa::EpsNfa(Grammar& g) :
	g_(g)
{

}

statusPtr EpsNfa::makeNewNode(Item& it) {
	int key = nextLabel();
	statusPtr s(new Status(key, it), std::bind(&EpsNfa::deleteStatusPtr, this, placeholders::_1));
	allStatus_[key] = s;
	return s;
}

edgePtr EpsNfa::makeNewEdge(const statusPtr& from, const statusPtr& to) {
	int key = nextLabel2();
	edgePtr e(new Edge(key, from, to), bind(&EpsNfa::deleteEdgePtr, this, placeholders::_1));
	allEdges_[key] = e;
	return e;
}

/*
 * processStatus
 */
void EpsNfa::processStatus(statusPtr& node, symbolPtr& s,map<rulePtr, statusPtr>& result)
{
	if (s->isNonTerminal()) {
		shared_ptr<vector<rulePtr>> vec = g_.findRules(s);
		for (auto r : *vec) {
			edgePtr e = makeNewEdge(node, result[r]);
			node->outEdges.push_back(e);
			result[r]->inEdges.push_back(e);
		}
	}
}
/*
 * buildEpsNfa ���ڹ���һ����-Nfa.
 */
statusPtr EpsNfa::buildEpsNfa() {
	/* ��һ��,�������еĹ���,����һ��״̬. */
	map<rulePtr, statusPtr> result;
	result.clear();
	statusPtr head;
	for (auto it = g_.begin(); it != g_.end(); ++it) {
		statusPtr node = makeNewNode(Item(*it, 0));
		if ((*it)->leftHandSymbol() == g_.startSymbol_) {
			head = node; /* ��¼����ʼ�Ľڵ� */
		}
		result[*it] = node;
	}
	/* �ڶ���,�ӿ�ʼ״̬��ʼ,һ��һ��������״̬ */
	for (auto pr : result) {
		rulePtr r = pr.first;
		statusPtr prevNode = pr.second;  /* ���ڼ�¼ǰһ��node */
		symbolPtr symbol = r->findNthElem(0);
		size_t length = r->rightHandLength();
		processStatus(prevNode, symbol, result);

		for (size_t i = 1; i <= length; ++i) {
			statusPtr node = makeNewNode(Item(r, i));
			/* ǰһ���ڵ㾭��symbol���Ե�����һ�ڵ� */
			edgePtr e = makeNewEdge(prevNode, node);
			prevNode->outEdges.push_back(e);
			node->inEdges.push_back(e);
			e->matchContent.push_back(symbol);
			if (i == length) { /* ���ܻ���ڶ������״̬, ����������ɹ�Լ�� */
				node->finalStatus = true;
				finalStatus_.insert(node);	/* ��¼����ֹ״̬ */
			}
			else {
				symbol = r->findNthElem(i);
				processStatus(node, symbol, result);
				prevNode = node;
			}
		}
	}
	//printStatusAndEdges();
	return head;
}

void EpsNfa::cleanEnv() {
	clearAllEdges();
	for (auto pr = allStatus_.begin(); pr != allStatus_.end(); ) {
		auto oldPr = pr++;
		statusPtr sPtr((*oldPr).second.lock());
		if (sPtr) {
			sPtr->outEdges.clear();
			sPtr->inEdges.clear();
		}
	}
	// tofix
	//assert(allStatus_.size() == 0);
}


/*
 * clearAllEdges ���allEdges��������б�,��������Ļ�,��Щ�߻�����������.
 * ��Ȼ,����㻹���иñߵ�ָ��,����Ȼ��������.
 */
void EpsNfa::clearAllEdges() {
	for (auto pr = allEdges_.begin(); pr != allEdges_.end(); ) {
		auto oldPr = pr++;
		edgePtr ePtr((*oldPr).second.lock());
		if (ePtr) {
			removeElement(ePtr->from->outEdges, ePtr);
			removeElement(ePtr->to->inEdges, ePtr);
			ePtr->from.reset();
			ePtr->to.reset();
		}
	}
	assert(allEdges_.size() == 0);
}

void EpsNfa::printStatusAndEdges(const wstring& title) {
	wcout << L"<<<<<<<<<<<<<<" << title.c_str() << L"<<<<<<<<<<<<" << endl;
	wcout << L"Status:" << endl;
	for (auto pr : allStatus_) {
		auto status(pr.second.lock());
		if (status) {
			wcout << L"label: " << status->label << endl;
			for (auto item : status->items) {
				wcout << item << endl;
			}
			wcout << endl;
		}
	}
	wcout << L"Edges:" << endl;
	for (auto pr : allEdges_) {
		auto edge(pr.second.lock());
		if (edge) {
			wcout << L"label: " << edge->label << endl;
			wcout << edge->from->label << L"==>" << edge->to->label;
			if (edge->isEps())
				wcout << L"	eps" << endl;
			else
				wcout << L"	" << *(edge->matchContent[0]) << endl;
		}
		wcout << endl;
	}
	wcout << L">>>>>>>>>>>>>>" << title.c_str() << L">>>>>>>>>>>>" << endl;
}



