/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ResponseCollection.Actions;

import library.ClientSocket;
import library.SpeechRecognition;

/**
 *
 * @author alvian
 */
public enum ActionType {
    DIRECTION_LEFT(1),
    DIRECTION_RIGHT(2),
    DIRECTION_BACKWARD(3),
    DIRECTION_FORWARD(4),
    DIRECTION_STOP(5),
    
    BROWSE_GOOGLE(6),
    BROWSE_YOUTUBE(7),
    BROWSE_FACEBOOK(8),
    BROWSE_TWITTER(9),
    BROWSE_ALLKPOP(10),
    
    MEANING(11),
    DEFINE(12),
    MISSION(13),
    STUDENT(14),
    LISTED(15),
    EQUALIZED(16),
    LOCATED(17),
    CHOSEN(18),
    BUILT(19),
    FOUNDED(20),
    
    FIND_ALVIAN(21),
    FIND_ANNA(22),
    FIND_JEFRI(23),
    FIND_LINTONG(24),
    FIND_MISS_MARGIANTI(25),
    FIND_MISTER_DENNIS(26),
    FIND_MISTER_ERY(27),
    FIND_MISTER_MAULANA(28),
    FIND_MISTER_MUSA(29),
    
    MACHINE_LEARNING_WHAT(30),
    MACHINE_LEARNING_WHO(31),
    MACHINE_LEARNING_NAME(32);

    private final int type;

    ActionType(int type) {
        this.type = type;
    }

    public int id() {
        return type;
    }

    public static void runAction(ClientSocket clientSocket, SpeechRecognition speech, int type, String userWords){
        System.out.println("[Action Type Code] : " + type);
        if(type == ActionType.DIRECTION_LEFT.id()){new Direction("left");}
        if(type == ActionType.DIRECTION_RIGHT.id()){new Direction("right");}
        if(type == ActionType.DIRECTION_BACKWARD.id()){new Direction("backward");}
        if(type == ActionType.DIRECTION_FORWARD.id()){new Direction("forward");}
        if(type == ActionType.DIRECTION_STOP.id()){new Direction("null direction, stopping");}
        if(type == ActionType.BROWSE_GOOGLE.id()){new Browsing(Browsing.Browse.GOOGLE);}
        if(type == ActionType.BROWSE_FACEBOOK.id()){new Browsing(Browsing.Browse.FACEBOOK);}
        if(type == ActionType.BROWSE_YOUTUBE.id()){new Browsing(Browsing.Browse.YOUTUBE);}
        if(type == ActionType.BROWSE_ALLKPOP.id()){new Browsing(Browsing.Browse.ALLKPOP);}
        if(type == ActionType.BROWSE_TWITTER.id()){new Browsing(Browsing.Browse.TWITTER);}
        if(type == ActionType.MEANING.id()){new CollegeInformation(speech, "meaning"  , userWords);}
        if(type == ActionType.DEFINE.id()){new CollegeInformation(speech, "defines", userWords);}
        if(type == ActionType.MISSION.id()){new CollegeInformation(speech, "mission", userWords);}
        if(type == ActionType.STUDENT.id()){new CollegeInformation(speech, "student", userWords);}
        if(type == ActionType.LISTED.id()){new CollegeInformation(speech, "listed", userWords);}
        if(type == ActionType.EQUALIZED.id()){new CollegeInformation(speech, "equalized", userWords);}
        if(type == ActionType.LOCATED.id()){new CollegeInformation(speech, "located", userWords);}
        if(type == ActionType.LOCATED.id()){new CollegeInformation(speech, "located", userWords);}
        if(type == ActionType.BUILT.id()){new CollegeInformation(speech, "built", userWords);}
        if(type == ActionType.FOUNDED.id()){new CollegeInformation(speech, "founded", userWords);}
        if(type == ActionType.FIND_ALVIAN.id()){FindPeople.find(clientSocket, FindPeople.People.ALVIAN);}
        if(type == ActionType.FIND_ANNA.id()){FindPeople.find(clientSocket, FindPeople.People.ANNA);}
        if(type == ActionType.FIND_JEFRI.id()){FindPeople.find(clientSocket, FindPeople.People.JEFRI);}
        if(type == ActionType.FIND_LINTONG.id()){FindPeople.find(clientSocket, FindPeople.People.LINTONG);}
        if(type == ActionType.FIND_MISS_MARGIANTI.id()){FindPeople.find(clientSocket, FindPeople.People.MISS_MARGIANTI);}
        if(type == ActionType.FIND_MISTER_DENNIS.id()){FindPeople.find(clientSocket, FindPeople.People.MISTER_DENNIS);}
        if(type == ActionType.FIND_MISTER_ERY.id()){FindPeople.find(clientSocket, FindPeople.People.MISTER_ERY);}
        if(type == ActionType.FIND_MISTER_MAULANA.id()){FindPeople.find(clientSocket, FindPeople.People.MISTER_MAULANA);}
        if(type == ActionType.FIND_MISTER_MUSA.id()){FindPeople.find(clientSocket, FindPeople.People.MISTER_MUSA);}
        if(type == ActionType.MACHINE_LEARNING_WHAT.id()){new MachineLearning(speech, userWords);}
        if(type == ActionType.MACHINE_LEARNING_WHO.id()){new MachineLearning(speech, userWords);}
        if(type == ActionType.MACHINE_LEARNING_NAME.id()){new MachineLearning(speech, userWords);}
    }
}
