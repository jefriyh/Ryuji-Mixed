package ryuji;

import java.util.Set;
import nlpeng.*;


/**
 * A simple keyword checker, that returns true if the word is not in the ban list.
 * An auxilliary list is also provided, however this method does not presently use it.
 *
 * @author Trejkaz
 */
public class SimpleKeywordChecker extends KeywordChecker {

    private Set banWords;
    private Set auxWords;
    public NlpEng nlp;

    public SimpleKeywordChecker(Set banWords, Set auxWords) {
        this.banWords = banWords;
        this.auxWords = auxWords;
        this.nlp = new NlpEng();
    }

    public boolean isKeyword(String symbol) {        
        Symbol s = new Symbol(symbol, false, null, null, null);        
        return isWord(symbol) && !banWords.contains(s);
    }

    public String setEntity(String symbol){        
        //System.out.println("\t"+symbol+"\t"+nlp.posTagger(symbol));
        //return nlp.posTagger(symbol);
        return null;
    }
}