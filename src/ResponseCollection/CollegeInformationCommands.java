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
public enum CollegeInformationCommands {
//    WHAT_1("what ", "what is the vision of gunadarma university ?", "", ActionType.COLLEGE_INFORMATION_WHAT.id()),
//    WHO_1("who", "who are you", "", ActionType.COLLEGE_INFORMATION_WHO.id()),
//    WHO_2("who", "who is the rector gunadarma ?", "", ActionType.COLLEGE_INFORMATION_WHO.id()),
    MEANING("meaning", "what is the meaning of the name gunadarma university ?", "", ActionType.MEANING.id()),
    DEFINE("defines", "what defines gunadarma ?", "", ActionType.DEFINE.id()),
    MISSION("mission", "what is the mission of gunadarma ?", "", ActionType.MISSION.id()),
    STUDENT("students", "what are gunadarma students activities ?", "", ActionType.STUDENT.id()),
    LISTED("listed", "when did gunadarma university got ' listed ' ?", "", ActionType.LISTED.id()),
    EQUALIZED("equalized", "when did gunadarma university got ' equalized ' ?", "", ActionType.EQUALIZED.id()),
    LOCATED_1("located", "where was gunadarma first located ?", "", ActionType.LOCATED.id()),
    LOCATED_2("located", "how did gunadarma firstly located ?", "", ActionType.LOCATED.id()),
    CHOSEN("chosen", "how did gunadarma name chosen ?", "", ActionType.CHOSEN.id()),
    BUILT("built", "how did gunadarma built ?", "", ActionType.BUILT.id()),
    FOUNDED("founded", "how did gunadarma first founded ?", "", ActionType.FOUNDED.id());

    private final String title = "Information about Gunadarma's Profile. Ask something like below : ";
    private final String said;
    private final String sample_said;
    private final String response;
    private final int action;

    CollegeInformationCommands(String said, String sample_said, String response, int action){
        this.said = said;
        this.sample_said = sample_said;
        this.response = response;
        this.action = action;
    }

    public static ArrayList items(){
        ArrayList<ArrayList> items = new ArrayList();
        for(int i=0;i<values().length;i++){
            ArrayList item = new ArrayList();
            item.add(values()[i].said);
            item.add(values()[i].sample_said);
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
