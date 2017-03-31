public class Goal {
    Symbol lhs; int pos; int length;

    public Goal(Goal goal) {//---
        lhs = goal.lhs; pos = goal.pos; length = goal.length;//---
    }//---

    public Goal(Symbol lhs, int pos, int length) {//---
        this.lhs = lhs; this.pos = pos; this.length = length;//---
    }//---

    public void doWork() {
        for (int n = 0; n < Grammar.size(); n++) {
            Rule r = Grammar.getRule(n);
            if (r.lhs.equals(lhs))
                (new RuleGoal(this, r)).doWork();
        }
    }

    public String toString() {//---
        return "lhs = " + lhs//---
            + ", pos = " + pos + ", length = " + length;//---
    }//---

    public boolean equals(Object o) {//---
        if (! (o instanceof Goal)) return false;//---
        Goal goal = (Goal) o;//---
        return lhs.equals(goal.lhs) && (pos == goal.pos)//---
            && (length == goal.length);//---
    }//---

    public int hashCode() {//---
        return lhs.hashCode() + pos + length;//---
    }//---
}
