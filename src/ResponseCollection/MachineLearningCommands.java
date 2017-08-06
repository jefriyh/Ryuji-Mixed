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
public enum MachineLearningCommands {
    WHAT_1("what ", "what is the vision of gunadarma university ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_2("what", "what is your name ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    NAME_1("name", "who are you ?", "", ActionType.MACHINE_LEARNING_NAME.id()),
    WHO_2("who", "who is the rector gunadarma ?", "", ActionType.MACHINE_LEARNING_WHO.id());

    private final String title = "Machine Learning";
    private final String said;
    private final String sample_said;
    private final String response;
    private final int action;

    MachineLearningCommands(String said, String sample_said, String response, int action){
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
