public class TD {
    static Input           input;
    static RuleStack       ruleStack;
    static DottedGoalStack dottedGoalStack;
    static KnownRuleGoals  knownRuleGoals;
    private static int     countDerivations;

    public static void parse(Input input) {
        TD.input = input;
        ruleStack = new RuleStack();
        dottedGoalStack = new DottedGoalStack();
        knownRuleGoals = new KnownRuleGoals();
        countDerivations = 0;

        System.out.println("Parsing \"" + input
                + "\" of length " + input.length());
        (new Goal(Grammar.startSym, 0, input.length())).doWork();
        System.out.println(countDerivations + " derivation"
                + (countDerivations == 1 ? "" : "s")
                + " found for string \"" + input + "\"\n");
    }

    public static void parsingFound() {
        countDerivations++;
        System.out.println("Parsing found:\n" + ruleStack.toString());
    }
}
