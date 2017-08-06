/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ResponseCollection;

import ResponseCollection.Actions.ActionType;
import java.util.ArrayList;

/**
 *
 * @author alvian
 */
public enum ConversationCommands {
//    RYUJI("ryuji", "hello too"),
//    HI("hi", "hello too"),
//    HELLO("hello", "hello too"),
    NICE_TO_MEET_YOU("nice to meet you", "nice to meet you too", ActionType.NICE_TO_MEET_YOU.id()),
    CAN_YOU_HELP("can you help", "what can i do for you", ActionType.CAN_YOU_HELP.id()),
    GOOD_BYE("good bye", "good bye", ActionType.GOOD_BYE.id());
    
    private final String title = "Conversation";
    private final String said;
    private final String response;
    private final int action;
        
    ConversationCommands(String said, String response, int action){
        this.said = said;
        this.response = response;
        this.action = action;
    }
    
    
    
    
    public static ArrayList items(){
        ArrayList<ArrayList> items = new ArrayList();
        for(int i=0;i<values().length;i++){
            ArrayList item = new ArrayList();
            item.add(values()[i].said);
            item.add(values()[i].response);
            item.add(values()[i].action);
            item.add(values()[i].title);
            items.add(item);
        }
        return items;
    }
    
    public String said(){
        return said;
    }
    
    public String response(){
        return response;
    }
}
