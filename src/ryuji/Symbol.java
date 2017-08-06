package ryuji;

import java.util.ArrayList;
import java.util.List;

/**
 * Wrapper class for symbols.  This class solves several problems:
 * (*) Enables faster comparison of symbols, by internalising all strings so we can use == instead of equals().
 * (*) Enables faster HashMap lookup, by implementing hashCode as an identity hashmap, which allows the same
 * sort of performance as using IdentityHashMap, but without breaking the general contract of the Map interface.
 * (*) Enables faster checks of whether a word is a keyword, by caching this check when the symbol is created.
 *
 * TODO: Check if we really need this class.  The original needs for it have been put aside for now, perhaps
 * it is possible to address some of those above performance problems in the TrieNode and TrieNodeMap classes.
 *
 * @author Trejkaz
 */
public class Symbol {

    public static final Symbol START = new Symbol("<START>", false, null, null, null);
    public static final Symbol END = new Symbol("<END>", false, null, null, null);

    private String symbol;
    private boolean keyword;
    public String entity;
    
    List<String> head = new ArrayList<String>();
    List<String> tail = new ArrayList<String>();

    //public Symbol(String symbol, boolean keyword, String entity, List<String> head, List<String> tail) {
    public Symbol(String symbol, boolean keyword, String entity, String head, String tail) {
        this.symbol = symbol.intern();
        this.keyword = keyword;
        this.entity = entity;
        if (!this.head.contains(head)){
            //System.out.println("head: "+head);
            this.head.add(head);
        }
        if (!this.head.contains(tail)){ 
            //System.out.println("tail: "+tail);
            this.tail.add(tail);
        }        
    }

    public String toString() {
        return symbol;
    }

    public boolean equals(Object other) {
        // Because we intern them.
        //noinspection StringEquality
        return (other instanceof Symbol) && (symbol == ((Symbol) other).symbol);
    }

    public int hashCode() {
        return System.identityHashCode(symbol);
    }

    public boolean isKeyword() {
        return keyword;
    }
    
    public List<String> getHead(){
        return this.head;
    }
    
    public List<String> getTail(){
        return this.tail;
    }
    
    public String getEntity(){
        return this.entity;
    }
}
