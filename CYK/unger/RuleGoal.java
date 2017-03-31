public class RuleGoal extends Goal {

    Rule rule;

    public RuleGoal(RuleGoal ruleGoal) {//---
        super(ruleGoal); rule = ruleGoal.rule;//---
    }//---

    public RuleGoal(Goal goal, Rule rule) {//---
        super(goal); this.rule = rule;//---
    }//---

    public void doWork() {
        // avoid left recursion:
        if (TD.dottedGoalStack.contains(this)) return;
        // try to avoid rebuilding known parses:
        if (TD.knownRuleGoals.knownRuleGoalTable.containsKey(this)) {
            TD.knownRuleGoals.doWork(this); return;
        }
        TD.knownRuleGoals.startNewParsing(this);
        System.out.println("Trying rule goal " + toString());
        (new DottedGoal(this, 0, 0)).doWork();
    }

    public void doWorkAfterDone() {
        if (TD.dottedGoalStack.empty()) TD.parsingFound();
        else TD.dottedGoalStack.top().doWorkAfterMatch(length);
    }

    public String toString() {//---
        return super.toString() + ", rule: " + rule;//---
    }//---

    public boolean equals(Object o) {//---
        if (! (o instanceof RuleGoal)) return false;//---
        return this.rule == ((RuleGoal) o).rule && super.equals(o);//---
    }//---
}
