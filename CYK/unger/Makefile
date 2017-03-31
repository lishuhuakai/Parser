#
#		Makefile for Top-Down Unger Parser in Java
#

N =		4


RESULT =	res$(N)

SRC =		TopDownParser.java TD.java Rule.java RuleStack.java \
		Goal.java RuleGoal.java DottedGoal.java \
		DottedGoalStack.java KnownRuleGoals.java \
		Grammar.java ReadInput.java \
		Symbol.java Input.java ReadGrammar.java
TEX =		TopDownParser.java.tex Grammar.java.tex TD.java.tex \
		Goal.java.tex RuleGoal.java.tex DottedGoal.java.tex \
		KnownRuleGoals.java.tex

# using @ feature because parameter list is too long for MSDOS
OPTIONS =	@srctmp
REQUIRED =	srctmp

test:		$(SRC) $(REQUIRED)
		javac $(OPTIONS)
		java TopDownParser UserGrammar$(N) < UserInput$(N)

compile:	$(SRC) $(REQUIRED)
		javac $(OPTIONS)

srctmp:		Makefile
		echo $(SRC) > srctmp

compare:	compile Makefile $(RESULT)
		java TopDownParser UserGrammar$(N) < UserInput$(N) | tee newres | diff -h - $(RESULT)

compareAll:	# Java-style goal
		make N=1 compare
		make N=2 compare
		make N=3 compare
		make N=4 compare
		make N=5 compare
		make N=6 compare

lint:		$(SRC) $(REQUIRED)
		javac -Xlint $(OPTIONS)

tex:		$(TEX)

.PHONY:		clean fresh
clean:
		rm -f *.class *.java.tex srctmp newres core

fresh:		clean

TopDownParser.java.tex: TopDownParser.java ../prog2tex
Grammar.java.tex: Grammar.java ../prog2tex
TD.java.tex: TD.java ../prog2tex
Goal.java.tex: Goal.java ../prog2tex
RuleGoal.java.tex: RuleGoal.java ../prog2tex
DottedGoal.java.tex: DottedGoal.java ../prog2tex
KnownRuleGoals.java.tex: KnownRuleGoals.java ../prog2tex
