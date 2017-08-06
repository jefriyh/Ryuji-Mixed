package ryuji;

import java.util.List;
import java.util.ArrayList;
import java.util.Random;
import java.util.regex.Pattern;
import java.util.regex.Matcher;
import nlpeng.*;

/**
 * A splitter which splits on word boundaries, splitting the string into a list of 'word'
 * fragments and 'non-word' fragments.
 *
 * @author Trejkaz
 */
public class WordNonwordSplitter implements Splitter {

    /**
     * The regex pattern which defines word boundaries.
     */
    private static Pattern boundaryPattern = Pattern.compile("([\\w']+|[^\\w']+)");

    /**
     * Symbol factory for creating symbols.
     */
    private final SymbolFactory symbolFactory;

    /**
     * Creates the splitter.
     *
     * @param symbolFactory symbol factory for creating symbols.
     */
    public WordNonwordSplitter(SymbolFactory symbolFactory) {
        this.symbolFactory = symbolFactory;
    }

    public List<Symbol> split(String text) {        
        //String str = String.join(arr);
        NlpEng nlp = new NlpEng();        
        //String string = String.join(nlp.tokenize(text));        
                     
        List<Symbol> symbolList = new ArrayList<Symbol>();
        symbolList.add(Symbol.START);

        Matcher m = boundaryPattern.matcher(text);
        
        StringBuffer head = new StringBuffer();
        String string = null;
        StringBuffer tail = new StringBuffer();        
        
        while (m.find()) {
            String str = m.group().toUpperCase().intern();
            boolean keyword = symbolFactory.keyword(str);
            //System.out.println(str+"\t key: "+symbolFactory.keyword(str));
            //System.out.println("String: "+str);
            
            if ((string == null) && (keyword == true)){
                head.append(str);
            }
            else if ((string == null) && (keyword == false)){
                string = str;
                //System.out.println("\t"+string);
            }
            else if ((string != null) && (keyword == false)){
                tail.append(str);
            }
            else {
                Symbol s = symbolFactory.createSymbol(string, head.toString(), tail.toString());
                symbolList.add(s);
                
                //System.out.println("\t"+head+"\t"+string+"\t"+tail);
                
                string = str;
                head = tail;
                tail = new StringBuffer();
            }
            
            //------------------
            /*
            if (symbolFactory.keyword(str)==true){            
                Symbol s = symbolFactory.createSymbol(m.group().toUpperCase().intern());
                symbolList.add(s);            
            }
            */
        }
        Symbol s = symbolFactory.createSymbol(string, head.toString(), tail.toString());
        symbolList.add(s);
        symbolList.add(Symbol.END);
        return symbolList;
    }

    public String join(List<Symbol> symbols) {
        // Chop off the <START> and <END>
        symbols = symbols.subList(1, symbols.size() - 1);

        // Build up the rejoined list.
        StringBuffer result = new StringBuffer();        
        int randomInt;
        int i = 0;
        
        Symbol symbol1 = null;
        
        for (Symbol symbol : symbols) {
            if (symbol1 == null){
                //System.out.println("symbol1: null");
            } else {
                //System.out.println("symbol1: "+symbol1.toString());
            }            
            //System.out.println("symbol: "+symbol.toString());
            i++;
            
            if ( (symbol1 == null) && (symbol.head.isEmpty() == true)){
                //symbol is the first symbol
                //System.out.println("satu");
                result.append(symbol.toString().toLowerCase());
                result.append(" ");
            } else if ((symbol1 == null) && (symbol.head.isEmpty() == false)){
                //symbol is the first symbol
                //System.out.println("dua");
                Random randomGenerator = new Random();
                randomInt = randomGenerator.nextInt(symbol.head.size());
                
                result.append(symbol.head.get(randomInt));
                result.append(" ");
                result.append(symbol.toString().toLowerCase());
                result.append(" ");            
            } else if (symbol1 != null){
                //System.out.println("empat");
                //find the similarity between head current symbol and the tail previous symbol
                List<String> similar = new ArrayList<String>();
                similar = getSimilarEl(symbol.head, symbol1.tail);                

                if (similar.size() > 1){                                        
                    //random elements since the elements are more than one
                    Random randomGenerator = new Random();
                    randomInt = randomGenerator.nextInt(similar.size());
                    
                    result.append(similar.get(randomInt));
                    result.append(" ");

                    result.append(symbol.toString().toLowerCase());
                    result.append(" ");

                } else if (similar.size() == 1) {                    
                    //only have one element
                    //System.out.println("similar contains: "+similar.contains("null"));
                    //System.out.println("similar contains: "+similar.get(0));
                    
                    result.append(similar.get(0));
                    result.append(" ");
                    result.append(symbol.toString().toLowerCase());
                    result.append(" ");
                } else {                        
                    //System.out.println("No one similar");                    
                    
                    /*
                    Random randomGenerator = new Random();
                    randomInt = randomGenerator.nextInt(symbol.head.size());
                    
                    result.append(symbol.head.get(randomInt));
                    result.append(" ");
                    */
                    
                    result.append(symbol.toString().toLowerCase());
                    result.append(" ");
                }
                /*
                if ((i == symbols.size()) && (symbol.tail.size() != 0)){
                    Random randomGenerator = new Random();
                    randomInt = randomGenerator.nextInt(symbol.head.size());
                                        
                    result.append(symbol.tail.get(randomInt));
                    result.append(" ");
                }
                */
            }
            symbol1 = symbol;            
            
            /*
            
            if (one){
                symbol1 = symbol;
                
                System.out.println("symbol-1 : "+symbol.toString().toLowerCase());
                System.out.println("==========head-1============");
                symbol.head.forEach(System.out::println);
                System.out.println("======================");

                System.out.println("\n==========tail-1============");
                symbol.tail.forEach(System.out::println);
                System.out.println("======================");
                
                one = false;
            } else if ((i == symbols.size()) && (symbol.tail.isEmpty() == false)){
                int j = 0;
                do {
                    j++;
                    Random randomGenerator = new Random();
                    randomInt = randomGenerator.nextInt(symbol.tail.size());
                } while ((symbol1.tail.get(randomInt) == null)&& (j <= symbol1.tail.size()));

                result.append("x "+symbol.toString().toLowerCase());
                result.append(" ");
                
                if (symbol.tail.get(randomInt) != null){
                    result.append(symbol.tail.get(randomInt));
                    result.append(" ");
                }                                
            } else if ((i == symbols.size()) && (symbol.tail.isEmpty() == true)){
                result.append("y "+symbol.toString().toLowerCase());
                result.append(" ");
            } else {                
                one = true;
                
                System.out.println("symbol-2 : "+symbol.toString().toLowerCase());
                System.out.println("head size : "+symbol.head.size());
                System.out.println("tile size : "+symbol.tail.size());
                System.out.println("==========head-2============");
                symbol.head.forEach(System.out::println);
                System.out.println("======================");

                System.out.println("\n==========tail-2============");
                symbol.tail.forEach(System.out::println);
                System.out.println("======================");
                        
                if ( (first == true) && (symbol1.head.isEmpty() == true) ){
                    System.out.println("1");
                    result.append("a "+symbol1.toString().toLowerCase());
                    result.append(" ");
                    first = false;
                } else if ( (first == true) && (symbol1.head.isEmpty() == false) ){
                    //random element if there is more than one elements.    
                    System.out.println("2");
                    int j = 0;
                    do {
                        j++;
                        Random randomGenerator = new Random();
                        randomInt = randomGenerator.nextInt(symbol1.head.size());
                        //System.out.println("random: "+symbol1.head.get(randomInt));
                    } while ((symbol1.head.get(randomInt) == null) && (j <= symbol1.head.size()));

                    if (symbol1.head.get(randomInt) != null){
                        result.append(symbol1.head.get(randomInt));
                        result.append(" ");
                    }                    
                    result.append("b "+symbol1.toString().toLowerCase());
                    result.append(" ");
                    first = false;
                } else {
                    System.out.println("3");
                    /*
                            finding the similar element between 
                            current head symbol and previous tail symbol
                    */
                /*    
                    List<String> similar = new ArrayList<String>();
                    similar = getSimilarEl(symbol.head, symbol1.tail);
                    System.out.println("similar size: "+similar.size() );

                    if (similar.size() > 1){
                        System.out.println("3a");
                        //random elements since the elements are more than one
                        int j = 0;
                        do {
                            j++;
                            Random randomGenerator = new Random();
                            randomInt = randomGenerator.nextInt(similar.size());
                        } while ((similar.get(randomInt) == null)&& (j <= similar.size()));
                        
                        if (similar.get(randomInt) != null){
                            result.append(similar.get(randomInt));
                            result.append(" ");
                        }
                        
                        result.append("g "+symbol.toString().toLowerCase());
                        result.append(" ");
                        
                    } else if (similar.size() == 1) {
                        System.out.println("3b");
                        //only have one element
                        //System.out.println("similar contains: "+similar.contains("null"));
                        System.out.println("similar contains: "+similar.get(0));
                        if (!similar.contains("null")){
                            //result.append("("+similar.get(0)+")");
                            result.append(similar.get(0));
                            result.append(" ");
                        }
                        
                        result.append("m "+symbol.toString().toLowerCase());
                        result.append(" ");
                    } else {                        
                        System.out.println("No one similar");
                    }

                    first = false;
                }
            }
*/
        }
            
            
        result.deleteCharAt(result.length()-1);
        return result.toString();
    }
    
    public List<String> getSimilarEl(List<String> head, List<String> tail){
        List<String> similar = new ArrayList<String>();
        for (String str : head){
            if (tail.contains(str)){
                similar.add(str);
            }
        }
        return similar;
    }
}

