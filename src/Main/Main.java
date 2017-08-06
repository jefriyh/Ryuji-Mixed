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
        speechRecognition.setResponses(new ArrayList() {
            {
                add(ConversationCommands.items());
                add(FindPeopleCommands.items());
                add(CollegeInformationCommands.items());
                add(MachineLearningCommands.items());
            }
        });
        String name = null;
        String state = "socket";
        boolean first = true;
        Ryuji ryuji = new Ryuji();
        boolean initial = true;
        boolean opening = true;
        String userWords = null;
        while (opening) {
            System.out.println("[STATE APP] : " + state);
            if (speechRecognition.isSpeechRecognitionRunning()) {
                userWords = speechRecognition.getCurentUserWords();
            }
            if ((state.equals("socket")) && (name == null)) {
                if (first == true) {
                    clientSocket.runMessage("REG;JAVA;");
                    first = false;
                    // state = "wait_init";
                } else {
                    name = clientSocket.getMessage().toUpperCase();
                    //System.out.println("name: "+name);
                    if (name.contains("MISS")) {
                        name = name.replace("MISS", "MISS ");//"MISS "+name.substring(4);
                    } else if (name.contains("MISTER")) {
                        name = name.replace("MISTER", "MISTER ");//"MISTER "+name.substring(6);
                    } else {
                        name = null;
                    }
                    state = "greeting";
                }
            } else if (state.equals("wait_init")) {
                String regd;
                do {
                    regd = clientSocket.getMessage().toUpperCase();
                } while (!regd.contains("REGD"));
                state = "socket";
                speechRecognition.closeSpeechRecognition();
            } else if (state.equals("greeting")) {
                System.out.print("RYUJI> ");

                if (name != null) {
                    //call t2s
                    out.println(ryuji.greetings(name));
                    speechRecognition.getSpeechInstance().speak(ryuji.greetings(name));
                    speechRecognition.getSpeechInstance().speak("What can I do for you?");

                } else {
                    //call t2s
                    out.println(ryuji.greetings(name));
                    speechRecognition.getSpeechInstance().speak(ryuji.greetings(name));
                    name = "USER";
                }
                opening = true;
                speechRecognition.openSpeechRecognition();
                state = "conversation";
            } else if (state.equals("conversation")) {
                speechRecognition.openSpeechRecognition();
                if (userWords != null) {
                    if (ryuji.isCommand(userWords)) {
                        state = "command";
                    } else {
                        speechRecognition.start(userWords);
                    }
                } else {
                    userWords = speechRecognition.getUserWords();
                }
            } else if (state.equals("command")) {
                name = null;
                state = "socket";
                speechRecognition.closeSpeechRecognition();
            }

        }
    }
}
