import java.util.Stack;

public class DottedGoalStack extends Stack<DottedGoal> {
    private static final long serialVersionUID = 1;//--
    public DottedGoal top() {return peek();}

    public String toString() {
        String res = "Stack: (" + size() + " dotted goals)\n";
        for (int i = 0; i < size(); i++) {
            res += elementAt(i).toString() + "\n";
        }
        return res;
    }
}
