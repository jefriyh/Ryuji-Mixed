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
    WHAT_3("what", "what program does gunadarma offer ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_4("what", "what is professional study program ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_5("what", "what is bachelor program ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_6("what", "what is postgraduate program ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_7("what", "what information technology program offer to study ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_8("what", "what health program offer to study ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_9("what", "what faculty of computer science program offer to study ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_10("what", "what economy faculty program offer to study ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_11("what", "what faculty of industrial technology program offer to study ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_12("what", "what faculty of civil and planning engineering program offer to study ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_13("what", "what faculty of psychology program offer to study ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_14("what", "what faculty of letters program offer to study ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_15("what", "what postgraduate magisterial program offer to study ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_16("what", "what postgraduate doctoral program offer to study ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_17("what", "what is magisterial program ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_18("what", "what is doctoral program ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_19("what", "what is doctoral program in information technology ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_20("what", "what is doctoral program in economics ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_21("what", "what is informatics management professional study program ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHAT_22("what", "what is computer engineering professional study program ?", "", ActionType.MACHINE_LEARNING_WHAT.id()),
    WHO_1("who", "who are you ?", "", ActionType.MACHINE_LEARNING_WHO.id()),
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
