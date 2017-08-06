package ryuji;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;
import java.util.List;

/**
 * Class for static utility methods.
 *
 * @author Trejkaz
 */
public class Utils {

    public static Map<String,String> readStringMapFromFile(String filename) throws IOException {
        Map<String,String> map = new HashMap<String,String>();
        
        BufferedReader reader = new BufferedReader(new FileReader(filename));
        String line;
        while ((line = reader.readLine()) != null) {
            line = line.trim();
            if (line.length() == 0) {
                continue;
            }
            if (line.charAt(0) == '#') {
                continue;
            }
            String[] words = line.split("\t");
            if (words.length != 2) { 
                continue;
            }
            map.put(words[0].toUpperCase(), words[1].toUpperCase());
        }
        reader.close();

        return map;        
    }
    
    public static Map<Symbol,Symbol> readSymbolMapFromFile(String filename) throws IOException {
        Map<Symbol,Symbol> map = new HashMap<Symbol,Symbol>();

        BufferedReader reader = new BufferedReader(new FileReader(filename));
        String line;
        while ((line = reader.readLine()) != null) {
            line = line.trim();
            if (line.length() == 0) {
                continue;
            }
            if (line.charAt(0) == '#') {
                continue;
            }
            String[] words = line.split("\t");
            if (words.length != 2) { 
                continue;
            }
            map.put(new Symbol(words[0].toUpperCase(), false, null, null, null), new Symbol(words[1].toUpperCase(), false, null, null, null));
        }
        reader.close();

        return map;
    }

    public static Set<Symbol> readSymbolSetFromFile(String filename) throws IOException {
        HashSet<Symbol> set = new HashSet<Symbol>();

        BufferedReader reader = new BufferedReader(new FileReader(filename));
        String line;
        while ((line = reader.readLine()) != null) {
            line = line.trim();
            if (line.length() == 0) {
                continue;
            }
            if (line.charAt(0) == '#') {
                continue;
            }
            set.add(new Symbol(line.toUpperCase(), false, null, null, null));
        }
        reader.close();

        return set;
    }
    
    public static List<String> readStringListFromFile(String filename) throws IOException {
        List<String> greet = new ArrayList<String>();

        BufferedReader reader = new BufferedReader(new FileReader(filename));
        String line;
        while ((line = reader.readLine()) != null) {
            line = line.trim();
            if (line.length() == 0) {
                continue;
            }
            if (line.charAt(0) == '#') {
                continue;
            }
            greet.add(line.toUpperCase());
        }
        reader.close();

        return greet;
    }

    public static boolean equals(List l1, List l2) {
        if (l1.size() != l2.size()) {
            return false;
        }
        Iterator il1 = l1.iterator();
        Iterator il2 = l2.iterator();
        while (il1.hasNext()) {
            if (!il1.next().equals(il2.next())) {
                return false;
            }
        }
        return true;
    }
}
