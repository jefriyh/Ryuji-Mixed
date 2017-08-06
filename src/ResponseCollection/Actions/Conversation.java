/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ResponseCollection.Actions;

import library.ClientSocket;

/**
 *
 * @author alvian
 */
public class Conversation {
    
    public enum Speak {
        NICE_TO_MEET_YOU(null),
        CAN_YOU_HELP_ME(null),
        GOOD_BYE("DATA;EXIT");
        
        private String runAction;
        
        Speak(String runAction){
            this.runAction = runAction;
        }
        
        private String getAction(){
            return runAction;
        }
    }
    
    public static void tell (ClientSocket clientSocket, Speak speak) {
        System.out.println("[Conversation Action] : " + speak.getAction());
        if (speak.getAction() != null) {
            clientSocket.runMessage(speak.getAction());
        }        
    }
}
