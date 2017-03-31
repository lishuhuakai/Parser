import java.util.ArrayList;

public class Grammar {
    private static ArrayList<Rule> ruleList = new ArrayList<Rule>();
    public static Symbol startSym = null;

    public static void addRule(Symbol lhs, Symbol[] rhs) {//---
        ruleList.add(new Rule(lhs, rhs));//---
        if (startSym == null) startSym = lhs;//---
    }//---

    public static Rule getRule(int n) {return ruleList.get(n);}
    public static int size() {return ruleList.size();}//---

    public static void load(String filename) {
        ReadGrammar.readGrammar(filename);
    }

    public static void parse() {
        Input s;
        while ((s = ReadInput.readInput()) != null) TD.parse(s);
    }
}
