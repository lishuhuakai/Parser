import java.io.FileReader;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.StreamTokenizer;
import java.util.ArrayList;

public class ReadGrammar {

    public static void readGrammar(String filename) {
        try {
            BufferedReader in = new BufferedReader(new FileReader(filename));
            StreamTokenizer tokenizer = new StreamTokenizer(in);
            int token;

            tokenizer.eolIsSignificant(true); tokenizer.lowerCaseMode(false);
            tokenizer.ordinaryChar('"'); tokenizer.ordinaryChar('\'');
            tokenizer.commentChar('#');
            for (;;) {
                do {
                    token = tokenizer.nextToken();
                } while (token == StreamTokenizer.TT_EOL);
                if (token == StreamTokenizer.TT_EOF)
                    break;
                if (token != StreamTokenizer.TT_WORD)
                    throw new IOException("Non-terminal expected");
                Symbol lhs = new Symbol(tokenizer.sval);
                token = tokenizer.nextToken();
                if (token != ':')
                    throw new IOException(": expected");
                ArrayList<Symbol> rhsList = new ArrayList<Symbol>();
                for (;;) {
                    token = tokenizer.nextToken();
                    if (token == StreamTokenizer.TT_EOF)
                        throw new IOException("Unexpected EOF");
                    if (token == StreamTokenizer.TT_EOL)
                        break;
                    if (token == StreamTokenizer.TT_WORD)
                        rhsList.add(new Symbol(tokenizer.sval));
                    else
                        rhsList.add(new Symbol(Character.toString((char) token)));
                }
                Grammar.addRule(lhs, rhsList.toArray(new Symbol[rhsList.size()]));
            }
        } catch(IOException e) {
            System.err.println("Got IOException while reading grammar: " + e);
            e.printStackTrace();
            System.exit(1);
        }
    }
}
