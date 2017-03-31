import java.util.Hashtable;

public class Symbol {
    private String id;

    public Symbol(String id) {
        this.id = id;
    }

    public boolean equals(Symbol s) {
        return id.equals(s.id);
    }

    public int hashCode() {
        return id.hashCode();
    }

    public String toString() {
        return id;
    }

    private static String toPossiblySpacedString(boolean sp, Symbol sym[]) {
        String res = new String();
        for (int n = 0; n < sym.length; n++) {
            res += (!sp || res.length() == 0 ? "" : " ") + sym[n].id;
        }
        return res;
    }
 
    public static String toString(Symbol sym[]) {
        return toPossiblySpacedString(false, sym);
    }

    public static String toSpacedString(Symbol sym[]) {
        return toPossiblySpacedString(true, sym);
    }

}
