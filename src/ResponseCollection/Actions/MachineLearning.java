/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ResponseCollection.Actions;

import java.io.IOException;
import library.SpeechRecognition;
import ryuji.Ryuji;

/**
 *
 * @author alvian
 */
public class MachineLearning {
    public MachineLearning(SpeechRecognition speech, String words) {
        try {
            Ryuji ryuji = new Ryuji();
            String answer = null;
            String question = null;
            if (words.contains("who are you")) {
                question = "what is your name";
            } else {
                question = words;
            }
            System.out.println("[REAL TEXT] : " + words);
            System.out.println("[TEXT OVERRIDE] : " + words.contains("who are you"));
            System.out.println("[OVERRIDE WITH] : " + question);
            System.out.println("[REQUEST] : " + question);
            System.out.println("[RYUJI] : thinking...");
            answer = ryuji.formulateReply(question);
            if (answer != null) {
                System.out.println("[RYUJI] : " + answer);
                speech.speak(answer);
            }
        } catch (IOException e) {

        }
    }
}
