#include	<stdio.h>
#include	<string.h>

#include	"TD_parser.h"

//#define	WFST
//#undef	CHARTEST

#define	MaxRules		100
#define	MaxRhsLength		10
#define	MaxInputLength		100
#define	ArraySize		1000 /* for all stacks and lists */

						/* SYMBOLS */
#ifndef	CHARTEST
typedef char SymbolType;
#else
typedef struct{char ch;} SymbolType;
#endif

static SymbolType NoSymbol = {'\0'};

static SymbolType Char2Symbol(char ch) {
#ifndef	CHARTEST
	return ch;
#else
	SymbolType res;

	res.ch = ch;
	return res;
#endif
}

static int SymbolsEqual(SymbolType s1, SymbolType s2) {
#ifndef	CHARTEST
	return s1 == s2;
#else
	return s1.ch == s2.ch;
#endif
}

static void PrintSymbol(SymbolType s) {
#ifndef	CHARTEST
	printf("%c", s);
#else
	printf("%c", s.ch);
#endif
}

static void PrintSymbols(SymbolType sym[]) {
	int n = 0;

	while (!SymbolsEqual(sym[n], NoSymbol)) {
		PrintSymbol(sym[n++]);
	}
}

						/* INPUT */
typedef SymbolType InputStringType[MaxInputLength];

static InputStringType InputString;
static int InputLength;

static void InitInputString(const char *inp) { // 对输入的字符进行处理
	int n = 0;
	while (inp[n] != '\0') {
		InputString[n] = Char2Symbol(inp[n]);
		n++;
	}
	InputString[n] = NoSymbol; // NoSymbol就是换行符啦。
	InputLength = n;
}

static void PrintInputString(void) {
	PrintSymbols(InputString);
}
						/* RULES */

typedef struct {SymbolType Sym[MaxRhsLength];} RhsType; // 右操作数类型
typedef struct {SymbolType Lhs; RhsType Rhs;} RuleType; // rule类型

static RuleType Grammar[MaxRules]; // Grammer由众多的rule组成
static int NRules;
static SymbolType StartSymbol; // 定义了开始符号

void StoreStartSymbol(char S) { // 这只是一个辅助函数而已，将字符转变为Symbol
	StartSymbol = Char2Symbol(S);
}

void StoreRule(char lhs, const char *rhs) { // C真的很有函数式编程的风味
	RuleType *r = &Grammar[NRules++]; //Grammer是一个静态类型，取得地址，往Grammer里面存储数据
	int n = 0;

	r->Lhs = Char2Symbol(lhs);
	while (rhs[n] != '\0') {
		r->Rhs.Sym[n] = Char2Symbol(rhs[n]);
		n++;
	}
	r->Rhs.Sym[n] = NoSymbol;
}

static void PrintRule(int nmb) { // 输出rule
	RuleType *r = &Grammar[nmb];

	PrintSymbol(r->Lhs);
	printf(" -> \"");
	PrintSymbols(r->Rhs.Sym);
	printf("\"");
}

						/* GOALS */

typedef struct { // 这就是所谓的Goal，居然只有一个左操作数，pos以及len
	SymbolType Lhs; int Pos; int Len;
} GoalType;

static GoalType NewGoal(SymbolType lhs, int pos, int len) { // 构建新的Goal
	GoalType gl;

	gl.Lhs = lhs; gl.Pos = pos; gl.Len = len;
	return gl;
}

static int GoalsEqual(GoalType goal1, GoalType goal2) { // 如何判定相等呢？三个数都要相等才行。
	return (SymbolsEqual(goal1.Lhs, goal2.Lhs))
		&& (goal1.Pos == goal2.Pos) && (goal1.Len == goal2.Len);
}

static void PrintGoal(GoalType goal) {
	printf("lhs = ");
	PrintSymbol(goal.Lhs);
	printf(", pos = %d, length = %d", goal.Pos, goal.Len);
}

						/* RULE GOALS */

typedef struct { // 话说Nmb到底是什么东西
	GoalType Goal; int Nmb;
	/* actually we would want the Rhs added here, but that does not
	   identify the rule unambiguously. */
} RuleGoalType;

static RuleGoalType NewRuleGoal(GoalType goal, int nmb) {
	RuleGoalType rg;

	rg.Goal = goal; rg.Nmb = nmb;
	return rg;
}

static void PrintRuleGoal(RuleGoalType rgoal) {
	PrintGoal(rgoal.Goal);
	printf(", rule: ");
	PrintRule(rgoal.Nmb);
}

static int RuleGoalsEqual(RuleGoalType rgoal1, RuleGoalType rgoal2) {
	return GoalsEqual(rgoal1.Goal, rgoal2.Goal)
		&& (rgoal1.Nmb == rgoal2.Nmb);
}

						/* DOTTED GOALS */

typedef struct { // dotted goals是什么玩意？
	RuleGoalType RuleGoal; int RhsUsed; int InpUsed; // 这玩意和Progress很像
} DottedGoalType;

static DottedGoalType NewDottedGoal(RuleGoalType rgoal, int rhs, int inp) {
	DottedGoalType dg;

	dg.RuleGoal = rgoal; dg.RhsUsed = rhs; dg.InpUsed = inp;
	return dg;
}

static void PrintDottedGoal(DottedGoalType dgoal) {
	PrintRuleGoal(dgoal.RuleGoal);
	printf(", RhsUsed = %d; InpUsed = %d", dgoal.RhsUsed, dgoal.InpUsed);
}

						/* PARSING STACKS */

static DottedGoalType Stack[ArraySize]; // progress的栈
static int NDottedGoals; // 这个应该是指示器吧！

static int RuleGoalIsToBeAvoided(RuleGoalType rgoal) {
	int i;

	for (i = 0; i < NDottedGoals; i++) {
		DottedGoalType *dg = &Stack[i];

		if (RuleGoalsEqual(dg->RuleGoal, rgoal)) return 1;
	}
	return 0;
}

static void PrintStack(void) {
	int i;

	printf("Stack: (%d dotted goals)\n", NDottedGoals);
	for (i = 0; i < NDottedGoals; i++) {
		PrintDottedGoal(Stack[i]);
	}
}

static int RuleStack[ArraySize];
static int NRulesStacked;
static int NDerivations;

static void PrintRulesStack(void) {
	int r;

	for (r = 0; r < NRulesStacked; r++) {
		PrintRule(RuleStack[r]); printf("\n");
	}
	printf("\n");
}

					/* HANDLING OF KNOWN PARSINGS */
#ifdef	WFST
typedef struct {int Start; int End;} KnownParsingType;

typedef struct {
	RuleGoalType RuleGoal;
	KnownParsingType KnownParsing[ArraySize];
	int NKnownParsings;
	int StartParsing; /* temporary variable ???? */
} KnownRuleGoalType;

static KnownRuleGoalType KnownRuleGoalList[ArraySize];
static int NKnownRuleGoals;
static int KnownRuleList[ArraySize];
static int NKnownRules;
#endif

static void InitKnownRuleGoals(void) {
#ifdef	WFST
	NKnownRuleGoals = 0; NKnownRules = 0;
#endif
}

#ifdef	WFST
static int KnownRuleGoalNumber(RuleGoalType rgoal) {
	int n;

	for (n = 0; n < NKnownRuleGoals; n++) {
		if (RuleGoalsEqual(KnownRuleGoalList[n].RuleGoal, rgoal))
			return n;
	}
	return -1;
}
#endif

static void StartNewKnownRuleGoal(RuleGoalType rgoal) {
#ifdef	WFST
	KnownRuleGoalType *kg = &KnownRuleGoalList[NKnownRuleGoals++];

	kg->RuleGoal = rgoal;
	kg->StartParsing = NRulesStacked;
	kg->NKnownParsings = 0;
#endif
}

static void RecordKnownParsing(void) { // 这个函数主要是在干什么？你可以说这玩意什么也没干，真的
#ifdef	WFST // WFST是优化过后的代码
	int tosRuleNmb = KnownRuleGoalNumber(Stack[NDottedGoals-1].RuleGoal);
	KnownRuleGoalType *kg = &KnownRuleGoalList[tosRuleNmb];
	KnownParsingType *kp = /* new known parsing */
		&kg->KnownParsing[kg->NKnownParsings++];
	int i;

	kp->Start = NKnownRules;
	for (i = kg->StartParsing; i < NRulesStacked; i++) {
		KnownRuleList[NKnownRules++] = RuleStack[i];
	}
	kp->End = NKnownRules;
#endif
}

static int RuleGoalIsKnown(RuleGoalType rgoal) {
#ifdef	WFST
	return KnownRuleGoalNumber(rgoal) != -1;
#else
	return 0;
#endif
}

static void DoTopOfStackAfter(int len);
static void ParsingFound(void);

static void DoKnownRuleGoal(RuleGoalType rgoal) {
#ifdef	WFST
	KnownRuleGoalType *kg = &KnownRuleGoalList[KnownRuleGoalNumber(rgoal)];
	int i;

	for (i = 0; i < kg->NKnownParsings; i++) {
		KnownParsingType *kp = &kg->KnownParsing[i];
		int oldNRulesStacked = NRulesStacked;
		int j;

		for (j = kp->Start; j < kp->End; j++) {
			RuleStack[NRulesStacked++] = KnownRuleList[j];
		}
		if (NDottedGoals == 0) ParsingFound();
		else DoTopOfStackAfter(rgoal.Goal.Len);
		NRulesStacked = oldNRulesStacked;
	}
#endif
}

						/* THE AUTOMATON */

static void InitParser(void) {
	NDottedGoals = 0; NRulesStacked = 0; NDerivations = 0;
	InitKnownRuleGoals();
}

static void ParsingFound(void) {
	NDerivations++;
	printf("Parsing found:\n");
	PrintRulesStack();
}

static void DoTopOfStack(void);

static void DoTopOfStackAfter(int len) { // 居然只有一个len，不科学啊！
	/* the non-terminal on top of Stack matched 'len' symbols */
	DottedGoalType *dg = &Stack[NDottedGoals-1]; // 获得栈顶部的元素

	/* advance top of Stack over that non-terminal */
	dg->RhsUsed += 1; dg->InpUsed += len;
	DoTopOfStack(); // 继续递归
	/* retract top of Stack */
	dg->RhsUsed -= 1; dg->InpUsed -= len; // 出来之后恢复原样
}

static void DoNextOnStack(void) { // 好吧，这个函数是重点所在
	/* the non-terminal in the goal on top of Stack was recognized */
	// 非终结符被识别了
	DottedGoalType oldTOS; // old top of stack

	RecordKnownParsing();
	oldTOS = Stack[--NDottedGoals]; // 先出栈，然后才赋值
	if (NDottedGoals == 0) ParsingFound(); // 解析成功了
	else DoTopOfStackAfter(oldTOS.RuleGoal.Goal.Len);
	Stack[NDottedGoals++] = oldTOS; // 居然还要进栈
}

static void TryAllRulesFor(GoalType goal); // 应该是从这里开始的吧！

static void TryAllLengthsFor(SymbolType lhs, int pos, int maxlen) {
	int len;

	for (len = 0; len <= maxlen; len++)
		TryAllRulesFor(NewGoal(lhs, pos, len));
}

static void DoTopOfStack(void) { // 这里是很重要的点
	DottedGoalType *dg = &Stack[NDottedGoals-1]; // 压根就没出栈,只是获取了栈的顶部的数据
	RuleGoalType *rg = &dg->RuleGoal; // 得到RuleGoal
	GoalType *gl = &rg->Goal; // 得到Goal
	SymbolType tosSymbol = /* active symbol on top of Stack */
		Grammar[rg->Nmb].Rhs.Sym[dg->RhsUsed];
	int pos = gl->Pos + dg->InpUsed;
	int len_left = gl->Len - dg->InpUsed;

	if (SymbolsEqual(tosSymbol, NoSymbol)) // 匹配到了ε
		if (len_left == 0) DoNextOnStack(); else /* match failed */;
	else
	if (len_left > 0 && SymbolsEqual(tosSymbol, InputString[pos]))
		DoTopOfStackAfter(1); /* 1 symbol was recognized */
	else TryAllLengthsFor(tosSymbol, pos, len_left);
}

static void DoUnknownRuleGoal(RuleGoalType rgoal) {
	printf("Trying rule goal "); PrintRuleGoal(rgoal); printf("\n");
	StartNewKnownRuleGoal(rgoal); 
	Stack[NDottedGoals++] = NewDottedGoal(rgoal, 0, 0); // 将东西入栈，记录下Progress
	RuleStack[NRulesStacked++] = rgoal.Nmb; // 这里的rule只记录下标是吧，也对，节省内存
	DoTopOfStack();
	--NRulesStacked; // 这是要出栈的节奏啊。
	--NDottedGoals;
}

static void DoRuleGoal(RuleGoalType rgoal) {
	if (RuleGoalIsToBeAvoided(rgoal)) return; // 如果会引起左递归，返回
	if (RuleGoalIsKnown(rgoal)) DoKnownRuleGoal(rgoal); // 如果这玩意的结果已经知道了
	else DoUnknownRuleGoal(rgoal); // 还不知道呢
}

static void TryAllRulesFor(GoalType goal) { // 尝试每一个rule
	int n;

	for (n = 0; n < NRules; n++)
		if (SymbolsEqual(Grammar[n].Lhs, goal.Lhs)) // 和我们的预想一样，从Grammer中搜寻规则
			DoRuleGoal(NewRuleGoal(goal, n)); // 从这里我们可以看到nmb指的是下标，这个规则在Grammer中的下标
}

void Parse(const char *inp) {
	InitInputString(inp);
	printf("Parsing \"");
	PrintInputString();
	printf("\" of length %d\n", InputLength);
	InitParser();
	TryAllRulesFor(NewGoal(StartSymbol, 0, InputLength));
	printf("%d derivation%s found for string \"",
	       NDerivations, (NDerivations == 1 ? "" : "s"));
	PrintInputString();
	printf("\"\n\n");
}

void InitGrammar(void) {
	StoreStartSymbol('S');
	StoreRule('S', "LSD");
	StoreRule('S', "");
	StoreRule('D', "d");
	StoreRule('L', "");
}

void DoParses(void) {
	Parse("dd");
}
						/* THE DRIVER */
int main(void) {
	NRules = 0;
	InitGrammar();
	DoParses();
	system("pause");
	return 0;
}
