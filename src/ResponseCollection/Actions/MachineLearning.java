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
            System.out.println("[RYUJI] : thinking...");
            if (overRideWords != null) {
               answer = ryuji.formulateReply(overRideWords);
            } else {
               answer = ryuji.formulateReply(words);
            }
            if (answer != null) {
                System.out.println("[RYUJI] : " + answer);
                speech.speak(answer);
            }
        } catch (IOException e) {

        }
    }
}
