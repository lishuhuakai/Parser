public class Rule {
    Symbol lhs;
    Symbol[] rhs;

    public Rule(Symbol lhs, Symbol[] rhs) {
        this.lhs = lhs;
        this.rhs = rhs;
    }

    public String toString() {
        return lhs.toString() + " -> " + Symbol.toSpacedString(rhs);
    }
}
