import java.util.Stack;

public class RuleStack extends Stack<Rule> {
    private static final long serialVersionUID = 1;//--

    public String toString() {
        String res = "";
        for (int r = 0; r < size(); r++) {
            res += elementAt(r).toString() + "\n";
        }
        return res;
    }
}
