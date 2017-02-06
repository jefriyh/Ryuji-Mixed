/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ResponseCollection.Actions;

import java.util.Arrays;
import library.SpeechRecognition;
import library.TagMatch;

/**
 *
 * @author alvian
 */
public class CollegeInfo2 {
    
    public CollegeInfo2(SpeechRecognition speech, String header, String words){
        TagMatch matchers = new TagMatch();
        matchers.setDataSet("/base-data/");
        String[] tags = words.split(" ");
        String response = matchers.getMatch(1, header, tags);
        System.out.println("tokens : " + Arrays.toString(tags));
        System.out.println("[Machine Learning] question : "  + words);
        System.out.println("[Machine Learning] answer   : "  + response);
        speech.speak(response);
    }
}
