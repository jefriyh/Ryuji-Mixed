package ryuji;

import java.util.*;
import java.io.IOException;
import java.io.FileReader;
import java.io.BufferedReader;
import java.text.ParseException;
import nlpeng.*;

/**
 * Main class implementing the main Ryuji engine.
 * Provides methods to train the brain, and to generate text
 * responses from it.
 *
 * @author Trejkaz
 */
public class Ryuji {

    // Fixed word sets.
    private final Map<Symbol, Symbol> swapWords;     // A mapping of words which will be swapped for other words.

    // Hidden Markov first_attempt.Model
    private final Model model;

    // Parsing utilities
    private final Splitter splitter;

    // Random Number Generator
    private final Random rng = new Random();        
    
    public Map<String, String> commands;
    
    public List<String> greeting = new ArrayList<String>();

    /**
     * Constructs the engine, reading the configuration from the data directory.
     *
     * @throws IOException if an error occurs reading the configuration.
     */
    public Ryuji() throws IOException {
        /*
         * 0. Initialise. Add the special "<BEGIN>" and "<END>" symbols to the
         * dictionary. Ex: 0:"<BEGIN>", 1:"<END>"
         *
         * NOTE: Currently debating the need for a dictionary.
         */
        //dictionary.add("<BEGIN>");
        //dictionary.add("<END>");

        String home = System.getProperty("user.home");        
        swapWords = Utils.readSymbolMapFromFile(home+"/data/megahal.swp");
        Set<Symbol> banWords = Utils.readSymbolSetFromFile(home+"/data/megahal.ban");        
        Set<Symbol> auxWords = Utils.readSymbolSetFromFile(home+"/data/megahal.aux");
        // TODO: Implement first message to user (formulateGreeting()?)
        greeting = Utils.readStringListFromFile(home+"/data/megahal.grt");
        
        commands = Utils.readStringMapFromFile(home+"/data/command.txt");
        
        SymbolFactory symbolFactory = new SymbolFactory(new SimpleKeywordChecker(banWords, auxWords));
        splitter = new WordNonwordSplitter(symbolFactory);
      model = new Model();

        BufferedReader reader = new BufferedReader(new FileReader(home+"/data/megahal2.trn"));
        String line;
        int trainCount = 0;
        while ((line = reader.readLine()) != null) {
            line = line.trim();
            if (line.length() == 0) {
                continue;
            }
            if (line.charAt(0) == '#') {
                continue;
            }
            trainOnly(line);
            trainCount++;
        }
        reader.close();
        //System.out.println("Trained with " + trainCount + " sentences.");
    }

    public String greetings(String name){
        StringBuffer intro = new StringBuffer();
        intro.append(" IT'S NICE TO MEET YOU ");
        
        try {            
            //get hour
            Time t = new Time();
            String time = t.formatTime(t.getTime());
            int hour = Integer.parseInt(time.substring(0, 2));
            
            if ((hour >= 0) && (hour < 12)){
                this.greeting.add("GOOD MORNING.");
            } else if ((hour >= 12) && (hour < 17)){
                this.greeting.add("GOOD AFTERNOON.");
            } else {
                this.greeting.add("GOOD EVENING.");
            }
            
            //random greeting
            int idx = (int)(Math.random() * this.greeting.size());
            
            StringBuffer str = new StringBuffer();
            str.append(this.greeting.get(idx)).append(intro);
                    
            //if there is no information of name
            if (!name.contains("USER NOT FOUND")){                
                str.append(name);
            }
            return str.toString();
        } catch (ParseException ex) {
            //if there is no information of name
            if (!name.equalsIgnoreCase("USER NOT FOUND")){                
                intro.append(name);
            }
            
            return intro.toString();
        }
    }
    
    
    public String getIntro(String gender){
        StringBuffer intro = new StringBuffer();
        intro.append("WHAT CAN I DO FOR YOU ");
        
        if ((gender != null)&&(gender.equalsIgnoreCase("female"))){
            intro.append("MA'AM");
        } else if ((gender != null) && (gender.equalsIgnoreCase("male"))){
            intro.append("SIR");
        }        
        return intro.toString();
    }
    
    /**
     * Trains on a single line of text.
     *
     * @param userText the line of text.
     */
    public void trainOnly(String userText) {
        // Split the user's line into symbols.
        List<Symbol> userWords = splitter.split(userText.toUpperCase());

        // Train the brain from the user's list of symbols.
        model.train(userWords);
    }

    public String getFirstWord(String str){
        if(str.contains(" ")){
            str = str.substring(0, str.indexOf(" "));
        }
        return str;
    }    
    
    public boolean isCommand(String input){
        boolean result = false;
        if (commands.containsKey(this.getFirstWord(input))){
            result = true;
        }
        return result;
    }
    
    public String[] command(String input){
        String[] commandPair = new String[2];
        
        commandPair[0] = commands.get(this.getFirstWord(input));
        commandPair[1] = input.substring(input.indexOf(" ")+1);
        return commandPair;
    }
    
    /**
     * Formulates a line back to the user, and also trains from the user's text.
     *
     * @param userText the line of text.
     * @return the reply.
     */
    public String formulateReply(String userText) {

        // Split the user's line into symbols.
        List<Symbol> userWords = splitter.split(userText.toUpperCase());
        
        // Train the brain from the user's list of symbols.
        model.train(userWords);

        // Find keywords in the user's input.
        List<Symbol> userKeywords = new ArrayList<Symbol>(userWords.size());
        
        //userKeywords = userWords;
        
        /*
        System.out.println("======================");
        userWords.forEach(System.out::println);
        System.out.println("======================");
        */        
        
        for (Symbol s : userWords) {
            if (s.isKeyword()) {
                Symbol swap = swapWords.get(s);
                if (swap != null) {
                    s = swap;
                }
                userKeywords.add(s);
            }
        }
        

        // Generate candidate replies.
        //int candidateCount = 0;
        double bestInfoContent = 0.0;
        List<Symbol> bestReply = null;
        int timeToTake = 1000 * 5; // 5 seconds.
        long t0 = System.currentTimeMillis();
        while (System.currentTimeMillis() - t0 < timeToTake) {
            //System.out.print("Generating... ");
            List<Symbol> candidateReply = model.generateRandomSymbols(rng, userKeywords);
            //candidateCount++;
            //System.out.println("Candidate: " + candidateReply);

            double infoContent = model.calculateInformation(candidateReply, userKeywords);            
            //System.out.println("infoContent="+infoContent);
            if (infoContent > bestInfoContent && !Utils.equals(candidateReply, userWords)) {
                bestInfoContent = infoContent;
                bestReply = candidateReply;
            }
        }
        //System.out.println("Candidates generated: " + candidateCount);
        //System.out.println("Best reply generated: " + bestReply);
        //System.out.println("info best reply: " + bestInfoContent);

        // Return the generated string, tacked back together.
        return (bestReply == null) ? null : splitter.join(bestReply);
    }
    
   
}
