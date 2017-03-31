import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.StreamTokenizer;
import java.util.ArrayList;

public class ReadInput {

    private static BufferedReader in;
    private static StreamTokenizer tokenizer;

    public static Input readInput() {
        try {
            if (in == null) {
                in = new BufferedReader(new InputStreamReader(System.in));
                tokenizer = new StreamTokenizer(in);
                tokenizer.eolIsSignificant(true);
                tokenizer.lowerCaseMode(false);
                tokenizer.ordinaryChar('"'); tokenizer.ordinaryChar('\'');
            }

            int token;

            for (;;) {
                ArrayList<Symbol> input = new ArrayList<Symbol>();
                for (;;) {
                    token = tokenizer.nextToken();
                    if (token == StreamTokenizer.TT_EOF) {
                        if (input.size() == 0)
                            return null;
                        throw new IOException("Unexpected EOF");
                    }
                    if (token == StreamTokenizer.TT_EOL)
                        break;
                    if (token == StreamTokenizer.TT_WORD)
                        input.add(new Symbol(tokenizer.sval));
                    else
                        input.add(new Symbol(Character.toString((char) token)));
                }
                return new Input(input.toArray(new Symbol[input.size()]));
            }
        } catch(IOException e) {
            System.err.println("Got IOException while reading input: " + e);
            e.printStackTrace();
        }
        return null;
    }
}
