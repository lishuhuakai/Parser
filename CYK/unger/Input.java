public class Input {
    private Symbol[] input;

    public Input(Symbol[] inp) {
        input = inp;
    }

    public int length() {return input.length;}

    public Symbol symbolAt(int pos) {return input[pos];}

    public String toString() {
        return Symbol.toString(input);
    }
}
