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
    public MachineLearning(SpeechRecognition speech, String words, String overRideWords) {
        try {
            Ryuji ryuji = new Ryuji();
            String answer = null;
            String question = null;
            if (overRideWords != null) {
               question = overRideWords;
            } else {
               question = words;
            }
            System.out.println("[TEXT] : " + overRideWords);
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
