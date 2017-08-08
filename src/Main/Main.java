/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package Main;

import ResponseCollection.Actions.MachineLearning;
import ResponseCollection.CollegeInformationCommands;
import ResponseCollection.ConversationCommands;
import ResponseCollection.FindPeopleCommands;
import ResponseCollection.MachineLearningCommands;
import java.io.IOException;
import static java.lang.System.out;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;
import library.ClientSocket;
import library.SpeechRecognition;
import ryuji.Ryuji;

/**
 *
 * @author parallels
 */
public class Main {

    /**
     * @param args the command line arguments
     */
    
    static String userWords = null;
    
    public static void main(String[] args) throws IOException {

        ClientSocket clientSocket = new ClientSocket();
        SpeechRecognition speechRecognition = new SpeechRecognition(args);
        speechRecognition.configure(
                SpeechRecognition.Config.WITH_VOICE,
                SpeechRecognition.Config.COLORED_CONSOLE,
                SpeechRecognition.Config.SHOW_RESPONSE,
                clientSocket
        );
        speechRecognition.setResponses(new ArrayList() {
            {
                add(ConversationCommands.items());
                add(FindPeopleCommands.items());
                add(CollegeInformationCommands.items());
                add(MachineLearningCommands.items());
            }
        });
        String name = null;
        String regd = null;
        String state = "socket";
        boolean first = true;
        Ryuji ryuji = new Ryuji();
        boolean initial = true;
        boolean opening = true;
        boolean secondRun = false;
        speechRecognition.initiateMicrophone();
        while (opening) {
            System.out.println("[STATE APP] : " + state);
            System.out.println("[SPeech Running] : " + speechRecognition.isSpeechRecognitionRunning());
            if (speechRecognition.isSpeechRecognitionRunning()) {
                userWords = speechRecognition.getCurentUserWords();
            }
            if (state == "socket") {
                name = null;
            }
            System.out.println("[Name] : " + name);
            if ((state.equals("socket")) && (name == null)) {
                System.out.println("[First] : " + first);
                if (first == true) {
                    if (!secondRun) {
                      clientSocket.runMessage("REG;JAVA;");
//                      secondRun = true;
                 
                    }
                     state = "wait_init";   
                    first = false;
                    System.out.println("[Begin STate] : "+ state);
                } else {
                    System.out.println("[Get First Name] : " + name);
                    System.out.println("[SECOND RUN] : " + secondRun);
                    if (!secondRun) {
                     name = clientSocket.getMessage().toUpperCase();
                     state = "greeting";
                     secondRun = true;
                     System.out.println("[Get Data Name] : " + name);
                    } else {
                       
                        state = "greeting";
                        if(!"MOVE".contains(regd)&&!"NO1".contains(regd) && !"NO2".contains(regd) && !"NO3".contains(regd) && !"REGD".contains(regd)){
                            name= regd;
                              clientSocket.runMessage("CONV;"+ryuji.greetings(name)+";");
                              speechRecognition.getSpeechInstance().speak(ryuji.greetings(name));
                        }
                        else if(!"MOVE".contains(regd)){
                            name = "";
                        }else{
                            name = "";
                        }
                        
                    }                    
                    //System.out.println("name: "+name);
                    
                    if (name.contains("MISS")) {
                        name = name.replace("MISS", "MISS ");//"MISS "+name.substring(4);
                    } else if (name.contains("MISTER")) {
                        name = name.replace("MISTER", "MISTER ");//"MISTER "+name.substring(6);
                    } else {
                        if ( !name.contains("NO1")){
                        if ( regd.contains("NO3")){
                            clientSocket.runMessage("CONV;the person you are looking for is not found;");
                            speechRecognition.getSpeechInstance().speak("the person you are looking for is not found");
                        } else if (regd.contains("NO2")){
                            clientSocket.runMessage("CONV;the person you are looking for is unregistered;");
                            speechRecognition.getSpeechInstance().speak("the person you are looking for is unregistered");
                        }
                        else if (regd.contains("MOVE")){
                            speechRecognition.getSpeechInstance().speak("moving");
                        }
                        }
                        name = null;
                       
                        
                    }
                    
                }
            } else if (state.equals("wait_init")) {
                System.out.println("[ending STate] : "+ state);
                regd= null;
                do {
                    regd = clientSocket.getMessage().toUpperCase();
                    System.out.println(regd);
                } while (regd.equals(null));
                state = "socket";
                speechRecognition.closeSpeechRecognition();
            } else if (state.equals("greeting")) {
                System.out.print("RYUJI> ");

                if (name != null && !secondRun) {
                    //call t2s
                    out.println(ryuji.greetings(name));
                    clientSocket.runMessage("CONV;"+"RYUJI;"+ryuji.greetings(name)+";");
                    speechRecognition.getSpeechInstance().speak(ryuji.greetings(name));
                    clientSocket.runMessage("CONV;"+"RYUJI;"+"What can I do for you?;");
                    speechRecognition.getSpeechInstance().speak("What can I do for you?");

                } else {
                    //call t2s
                    out.println(ryuji.greetings(name));
                    clientSocket.runMessage("CONV;"+"RYUJI;"+ryuji.greetings(name)+";");
                    speechRecognition.getSpeechInstance().speak(ryuji.greetings(name));
                    name = "USER";
                }
                opening = true;
                speechRecognition.openSpeechRecognition();
                state = "conversation";
            } else if (state.equals("conversation")) {
                userWords = speechRecognition.getUserWords();
                System.out.println("user words :" + userWords);
                speechRecognition.openSpeechRecognition();
                System.out.println("[words]" + userWords);
                if (userWords != null) {
                    clientSocket.runMessage("CONV;"+name+";"+userWords+";");
                    if (ryuji.isCommand(userWords) || userWords.toLowerCase().contains("find") || userWords.toLowerCase().contains("go ")) {
                        state = "command";
                        name = null;
                        speechRecognition.start(userWords);
                        speechRecognition.closeSpeechRecognition();
                        first = true;
                        System.out.println("[state ryuji] :"+state);
                    } 
                    else if(userWords.contains("good bye")){
                        secondRun = false;
                        clientSocket.runMessage("CONV;RYUJI;Bye Bye "+name+";");
                        speechRecognition.getSpeechInstance().speak("Bye Bye "+name);
                        state = "command";
                        name = null;
                        clientSocket.runMessage("DATA;EXIT;");
                        speechRecognition.closeSpeechRecognition();    
                    }
                        else {   
                        speechRecognition.start(userWords);
                    }
                    
                }
            } else if (state.equals("command")) {
                name = null;
                state = "socket";
                speechRecognition.closeSpeechRecognition();
            }
        }
    }
}
