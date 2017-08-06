/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package library;

/**
 *
 * @author parallels
 */
import Main.Main;
import com.sun.speech.freetts.Voice;
import com.sun.speech.freetts.VoiceManager;
import edu.cmu.sphinx.frontend.util.Microphone;
import edu.cmu.sphinx.jsgf.JSGFGrammar;
import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.util.props.ConfigurationManager;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import library.Speech.Color;
import library.Speech.Config;
import library.Speech.Get;

public class SpeechRecognition extends Speech {

    private final Recognizer recognizer;
    private JSGFGrammar jsgfGrammar;
    private final ConfigurationManager cm;
    private ColorizeConsole out;
    private VoiceManager vm;
    private Voice voice;
    private Response response;
    private ClientSocket clientSocket;
    private int open_voice = 0;
    private int colorize_console = 0;
    private int show_response = 0;
    public static boolean running = false;
    private String currentUserWords = null;
    private static boolean openSpeechRecognition = false;

    public SpeechRecognition(String[] args) {
        if (args.length > 0) {
            cm = new ConfigurationManager(args[0]);
        } else {
            cm = new ConfigurationManager(Main.class.getResource("config.xml"));
        }
        recognizer = (Recognizer) cm.lookup("recognizer");
        jsgfGrammar = (JSGFGrammar) cm.lookup("jsgfGrammar");
        recognizer.allocate();
    }

    public void configure(Speech.Config type, Speech.Config colored, Speech.Config response, ClientSocket _clientSocket) {
        open_voice = type.id();
        colorize_console = colored.id();
        show_response = response.id();
        clientSocket = _clientSocket;
        if (open_voice == 1) {
            setupVoice();
        }
        if (colorize_console == 1) {
            setupColoredConsole();
        }
        if (show_response == 1) {
            setupResponse();
        }
    }

    public void configure(Speech.Config type, Speech.Config colored) {
        open_voice = type.id();
        colorize_console = colored.id();
        show_response = Config.NO_RESPONSE.id();
        if (open_voice == 1) {
            setupVoice();
        }
        if (colorize_console == 1) {
            setupColoredConsole();
        }
    }

    public void configure(Speech.Config type) {
        open_voice = type.id();
        colorize_console = Config.NON_COLORED_CONSOLE.id();
        show_response = Config.NO_RESPONSE.id();
        if (open_voice == 1) {
            setupVoice();
        }
    }

    public void configure() {
        open_voice = Config.NO_VOICE.id();
        colorize_console = Config.NON_COLORED_CONSOLE.id();
        show_response = Config.NO_RESPONSE.id();
    }

    public void setResponses(ArrayList<ArrayList<ArrayList>> items) {
        if (items.size() > 0) {
            show_response = Config.SHOW_RESPONSE.id();
            setupResponse();
        }
        if (show_response == 1) {
            for (int i = 0; i < items.size(); i++) {
                response.add(items.get(i));
            }
        }
    }

    private void setupResponse() {
        response = new Response();
    }

    private void setupColoredConsole() {
        try {
            out = new ColorizeConsole(System.out);
        } catch (UnsupportedEncodingException ex) {
            output(Color.RED, Get.COLORING_CONSOLE_ERROR.respone());
        }
    }

    private void setupVoice() {
        vm = VoiceManager.getInstance();
        voice = vm.getVoice("kevin16");
        voice.setStyle("casual");
        voice.allocate();
        voice.setVolume(100);
        voice.setRate(130);
        voice.setPitch(130);
    }

    public void speak(String text) {
        if (open_voice == 1 && text != null) {
            voice.speak(text);
        }
    }

    public SpeechRecognition getSpeechInstance() {
        return this;
    }

    private void output(Color color, String text) {
        if (colorize_console == 1) {
            switch (color) {
                case GREEN:
                    out.green(text);
                    break;
                case YELLOW:
                    out.yellow(text);
                    break;
                case RED:
                    out.red(text);
                    break;
                case WHITE:
                    out.white(text);
                    break;
                case CYAN:
                    out.cyan(text);
                    break;
                case BLUE:
                    out.blue(text);
                    break;
                case BLACK:
                    out.black(text);
                    break;
            }
        } else {
            System.out.println(text);
        }
    }

    private String[] setOutput(String text) {
        String person = "You >> ";
        if (show_response == 1) {
            String result = text;
            System.out.println("[Recognition] : checking output...");
            for (int i = 0; i < response.size(); i++) {
                if (!result.equals(text)) {
                    break;
                }
                for (int j = 0; j < response.get(i).size(); j++) {
                    String commandType = response.getCommandType(i, j);
                    if (text.toLowerCase().contains(commandType)) {
                        System.out.println("[Recognition Text] : " + text.toLowerCase());
                        System.out.println("[Recognition Head] : " + commandType);
                        result = response.getResponse(i, j);
                        person = "Bot >> ";
                        break;
                    }
                }
            }
            System.out.println("[Recognition] : output taken");
            return new String[]{person, result};
        } else {
            return new String[]{person, text};
        }
    }

    private void checkAction(String text) {
        String result = text;
        int _i = 0;
        int _j = 0;
        boolean runAction = false;
        System.out.println("[Recognition] : checking action...");
        for (int i = 0; i < response.size(); i++) {
            if (runAction) {
                break;
            }
            for (int j = 0; j < response.get(i).size(); j++) {
                String commandType = response.getCommandType(i, j);
                if (text.toLowerCase().contains(commandType)) {
                    System.out.println("[Recognition] : action is ready");
                    result = text.toLowerCase();
                    _i = i;
                    _j = j;
                    runAction = true;
                    break;
                }
            }
        }
        System.out.println("[Recognition] : running action");
        if (runAction) {
            System.out.println("[Recognition] : action taken");
            response.setSpeechInstance(getSpeechInstance());
            response.setSocketInstance(clientSocket);
            response.setUserWords(result);
            response.shouldRunAction(_i, _j);
        }
    }

    public boolean isSpeechRecognitionRunning() {
        return openSpeechRecognition;
    }

    public void closeSpeechRecognition() {
        openSpeechRecognition = false;
    }

    public void openSpeechRecognition() {
        openSpeechRecognition = true;
    }

    public String getCurentUserWords() {
        return currentUserWords;
    }

    private boolean open_app = false;

    public String getUserWords() {
        try {
            Result result = recognizer.recognize();
            String resultText = result.getBestFinalResultNoFiller();
            if (resultText != null) {
                return resultText;
            } else {
                return "";
            }   
        } catch (Exception e) {
            return "";
        }
    }

    public void start(String words) {
        // start the microphone or exit if the programm if this is not possible
        Microphone microphone = (Microphone) cm.lookup("microphone");
//        output(Color.BLUE, "say something...");
        if (!microphone.startRecording()) {
            output(Color.RED, Get.MIC_ERROR.respone());
            recognizer.deallocate();
            System.exit(1);
        }
        // loop the recognition until the programm exits.
        if (openSpeechRecognition) {
            System.out.println("Recognition Looping...");
            System.out.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
            response.ListCommand();
            String resultText = words;
            currentUserWords = words;
            String output[] = setOutput(resultText);
            System.out.println("[Words] : " + resultText);
            if (!resultText.equals("")) {
                if (resultText.toLowerCase().contains("quit ")) {
                    output(Color.GREEN, Get.QUIT.respone());
                    voice.deallocate();
                    System.exit(0);
                } else {
                    if (!output[1].equals("")) {
                        output(Color.GREEN, output[0] + output[1]);
                    }
//                    output(Color.GREEN, output.toString());
                    if (output[0] != "You >> ") {
                        speak(output[1]);
                    }
                    checkAction(resultText);
                }
            } else {
                output(Color.YELLOW, "Bot >> " + Get.ERROR.respone());
            }
            System.out.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        }
    }
}

class Speech {

    public enum Config {
        WITH_VOICE(1),
        NO_VOICE(0),
        COLORED_CONSOLE(1),
        NON_COLORED_CONSOLE(0),
        SHOW_RESPONSE(1),
        NO_RESPONSE(0);

        private int id;

        Config(int id) {
            this.id = id;
        }

        int id() {
            return id;
        }
    }

    protected enum Color {
        YELLOW(0),
        GREEN(1),
        RED(2),
        BLUE(3),
        WHITE(4),
        CYAN(5),
        PURPLE(6),
        BLACK(7);

        private int id;

        Color(int id) {
            this.id = id;
        }

        int id() {
            return id;
        }
    }

    protected enum Get {
        MIC_ERROR("[Speech Recognition] : cannot start your microphone"),
        COLORING_CONSOLE_ERROR("[Speech Recognition] : error colorizing console"),
        ERROR("I can't hear what you said"),
        QUIT("quitting system...");

        private String text;

        Get(String text) {
            this.text = text;
        }

        String respone() {
            return text;
        }
    }
}
