public class TopDownParser {
    public static void main(String[] args) {
        String userGrammarName = "UserGrammar4";

        if (args.length > 1) {//---
            System.err.println("Usage: java TopDownParser [ <grammarfile> ]");//---
            System.exit(1);//---
        }//---
        if (args.length == 1)//---
            userGrammarName = args[0];//---

        Grammar.load(userGrammarName);
        Grammar.parse();
    }
}
