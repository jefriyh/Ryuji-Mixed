/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ryuji;

/**
 *
 * @author jefri
 */
import com.sun.speech.freetts.Voice;
import com.sun.speech.freetts.VoiceManager;
 
public class TextToSpeechConverter {
 
 // Some available voices are (kevin, kevin16, alan)
 private static final String VOICE_NAME_KEVIN = "kevin16";
 private final Voice voice;
 
 public TextToSpeechConverter() {
 
 VoiceManager vm = VoiceManager.getInstance(); 
 voice = vm.getVoice(VOICE_NAME_KEVIN);
 voice.setPitch((float)(57));
    voice.setPitchShift((float)(2.5));
    voice.setPitchRange((float)(5));
    voice.setStyle("casual");
    
 
 voice.allocate();

 }
 
 public void speak(String inputText) {
 
 if(inputText != null && !inputText.isEmpty()) {
 
 voice.speak(inputText);
 }
 }
 
}