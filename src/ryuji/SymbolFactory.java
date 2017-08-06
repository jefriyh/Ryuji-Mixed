package ryuji;

/**
 * Factory which creates symbols.
 *
 * @author Trejkaz
 */
public class SymbolFactory {
    /**
     * Checks whether symbols are keywords.
     */
    private final KeywordChecker checker;

    /**
     * Constructs the factory.
     *
     * @param checker checks whether symbols are keywords.
     */
    public SymbolFactory(KeywordChecker checker) {
        this.checker = checker;
    }

    /**
     * Creates a symbol.
     *
     * @param symbol the string value.
     * @param head
     * @param tail
     * @return the symbol.
     */
    public Symbol createSymbol(String symbol, String head, String tail) {
        //System.out.println("symbol:\t"+symbol+"\t"+checker.isKeyword(symbol));
        return new Symbol(symbol, checker.isKeyword(symbol), checker.setEntity(symbol), head, tail);
    }

    public boolean keyword(String symbol){
        return checker.isKeyword(symbol);
    }

}
