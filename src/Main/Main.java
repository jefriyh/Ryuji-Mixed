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
    public static void main(String[] args) throws IOException {
        
        ClientSocket clientSocket = new ClientSocket();
        SpeechRecognition speechRecognition = new SpeechRecognition(args);
        speechRecognition.configure(
                SpeechRecognition.Config.WITH_VOICE,
                SpeechRecognition.Config.COLORED_CONSOLE,
                SpeechRecognition.Config.SHOW_RESPONSE,
                clientSocket
        );
        String name = null;
        String state = "socket";
        boolean first = true;
        Ryuji ryuji = new Ryuji();
        boolean opening = true;
        while (opening) {
            if ((state.equals("socket")) && (name == null)) {
                if (first == true) {
                    clientSocket.runMessage("REG;JAVA;");
                    first = false;
                    // state = "wait_init";                  
                } else {
                    name = clientSocket.getMessage().toUpperCase();
                    if (name.contains("MISS")) {
                        name = name.replace("MISS", "MISS ");//"MISS "+name.substring(4);
                    } else {
                        name = name.replace("MISTER", "MISTER ");//"MISTER "+name.substring(6);
                    }
                    state = "greeting";
                }
            } else if (state.equals("wait_init")) {
                String regd;
                do {
                    regd = clientSocket.getMessage().toUpperCase();
                } while (!regd.contains("REGD"));
                state = "socket";
            } else if (state.equals("greeting")) {
                System.out.print("RYUJI> ");
                if (!name.contains("NO")) {
                    //call t2s
                    System.out.println(ryuji.greetings(name));
                    speechRecognition.getSpeechInstance().speak(ryuji.greetings(name));
                    speechRecognition.getSpeechInstance().speak("What can I do for you?");
                } else {
                    //call t2s                    
                    out.println(ryuji.greetings(name));
                    speechRecognition.getSpeechInstance().speak(ryuji.greetings(name));
                    name = "USER";
                }
                opening = false;
                state = "conversation";
            }
        }
        speechRecognition.setResponses(new ArrayList() {
            {
//                add(BasicCommands.items());
//                add(DirectionCommands.items());
//                add(BrowsingCommands.items());
                   add(ConversationCommands.items());
                  add(FindPeopleCommands.items());
                add(CollegeInformationCommands.items());
                add(MachineLearningCommands.items());
            }
        });
        speechRecognition.start();
    }
}
