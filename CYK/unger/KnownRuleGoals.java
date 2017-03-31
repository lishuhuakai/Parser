import java.util.ArrayList;
import java.util.Hashtable;

public class KnownRuleGoals {

  Hashtable<RuleGoal,KnownRuleGoal> knownRuleGoalTable
      = new Hashtable<RuleGoal,KnownRuleGoal>();
  Hashtable<RuleGoal, Integer> startParsingTable
      = new Hashtable<RuleGoal, Integer>();

  public void startNewParsing(RuleGoal ruleGoal) {
    startParsingTable.put(ruleGoal, new Integer(TD.ruleStack.size()));
    knownRuleGoalTable.put(ruleGoal, new KnownRuleGoal(ruleGoal));
  }

  public void recordParsing(RuleGoal ruleGoal) {
    knownRuleGoalTable.get(ruleGoal).record();
  }

  public void doWork(RuleGoal ruleGoal) {
    knownRuleGoalTable.get(ruleGoal).doWork();
  }

  private class KnownRuleGoal {
    RuleGoal ruleGoal;

    KnownRuleGoal(RuleGoal ruleGoal) {//---
      this.ruleGoal = ruleGoal;//---
    }//---

    ArrayList<KnownParsing> knownParsingSet =
        new ArrayList<KnownParsing>();

    void record() {
      knownParsingSet.add(new KnownParsing());
    }

    void doWork() {
      for (int i = 0; i < knownParsingSet.size(); i++) {
        knownParsingSet.get(i).doWork();
      }
    }

    private class KnownParsing {
      Rule [] knownParsing;
  
      KnownParsing() {
        int stackSizeAtStart =
          startParsingTable.get(ruleGoal).intValue();
        int stackSize = TD.ruleStack.size();
        knownParsing = new Rule[stackSize - stackSizeAtStart];
        for (int i = stackSizeAtStart, j = 0; i < stackSize; i++, j++) {
          knownParsing[j] = TD.ruleStack.elementAt(i);
        }
      }
  
      void doWork() {
        int oldStackSize = TD.ruleStack.size();
        for (int i = 0; i < knownParsing.length; i++) {
          TD.ruleStack.push(knownParsing[i]);
        }
        ruleGoal.doWorkAfterDone();
        TD.ruleStack.setSize(oldStackSize); // pop all
      }
  
    }
  }
}
