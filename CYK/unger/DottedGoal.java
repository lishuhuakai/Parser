public class DottedGoal extends RuleGoal {

    private int rhsUsed, inputUsed; // positions of dot in rhs and input

    public DottedGoal(RuleGoal ruleGoal, int rhs, int inp) {//---
        super(ruleGoal); rhsUsed = rhs; inputUsed = inp;//---
    }//---

    public void doWork() {
        TD.dottedGoalStack.push(this);
        TD.ruleStack.push(rule);
        doAsTopOfStack();
        TD.ruleStack.pop();
        TD.dottedGoalStack.pop();
    }

    public void doWorkAfterMatch(int matchedLength) {
        // advance the dotted goal over matched non-terminal and input
        rhsUsed += 1; inputUsed += matchedLength;
        doAsTopOfStack();
        // retract the dotted goal
        rhsUsed -= 1; inputUsed -= matchedLength;
    }

    private void doAsTopOfStack() { // 'this' is top of parsing stack
        int activePos = pos + inputUsed;
        int leftoverLength = length - inputUsed;

        if (rule.rhs.length == rhsUsed) { // rule exhausted
            if (leftoverLength == 0) {    // input exhausted
                TD.knownRuleGoals.recordParsing(this);
                TD.dottedGoalStack.pop();
                ((RuleGoal)this).doWorkAfterDone();
                TD.dottedGoalStack.push(this);
            }
        } else {
            Symbol rhsAtDot = rule.rhs[rhsUsed];
            if (leftoverLength > 0) {
                Symbol inputAtDot = TD.input.symbolAt(activePos);
                if (rhsAtDot.equals(inputAtDot)) doWorkAfterMatch(1);
            }
            for (int len = 0; len <= leftoverLength; len++)
                (new Goal(rhsAtDot, activePos, len)).doWork();
        }
    }

    public String toString() {//---
        return super.toString() + ", rhsUsed = " + rhsUsed//---
            + ", inputUsed = " + inputUsed;//---
    }//---
}
