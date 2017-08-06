/*****************************************************************************
*   Face Recognition using Eigenfaces
******************************************************************************

*   Main code for face recognition

******************************************************************************
*   by Anissa Lintang Ramadhani, th July 2017
*   alintangr@gmail.com
*
*   based on Shervin Emami, 5th Dec 2012
/// http://www.shervinemami.info/openCV.html
/// Ch8 of the book "Mastering OpenCV with Practical Computer Vision Projects"
/// Copyright Packt Publishing 2012.
/// http://www.packtpub.com/cool-projects-with-opencv/book
*****************************************************************************/
#include <sys/time.h>       //FD_SET, FD_ISSET, FD_ZERO macros
#include <sys/types.h>      //berisi definisi dari tipe data yang diperlukan sama socket
#include <sys/socket.h>     //berisi beberapa definisi structure yang diperlukan socket
#include <netinet/in.h>     //berisi nilai2 constant dan structure yang diperlukan internet domain addresses
#include <arpa/inet.h>

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctime>
#include <time.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>

// Include Python library
#include <python2.7/Python.h>

// Include OpenCV's C++ Interface
#include "opencv2/opencv.hpp"

// Include the rest of our code!
#include "detectObject.h"       // Easily detect faces or eyes (using Haar Cascades).
#include "preprocessFace.h"     // Easily preprocess face images, for face recognition.
#include "recognition.h"     // Train the face recognition system and recognize a person from an image.

#include "ImageUtils.h"      // Shervin's handy OpenCV utility functions.

// DEFINE FOR SOCKET
#define TRUE   1
#define FALSE  0
#define PORT 1500
// #define PORT 8888
#define MAX_CLIENTS 2 

using namespace cv;
using namespace std;


#if !defined VK_ESCAPE
    #define VK_ESCAPE 0x1B      // Escape character (27)
#endif

// Dummy array for checking the code
string listname[6]= 
{   "miss.anne",
    "miss.lintang",
    "mister.antoine",
    "mister.dnur",
    "mister.nicolas",
    "mister.valentin",  };


const char *facerecAlgorithm = "FaceRecognizer.Eigenfaces";

// Sets how confident the Face Verification algorithm should be to decide if it is an unknown person or a known person.
const float UNKNOWN_PERSON_THRESHOLD = 0.5f;

// Cascade Classifier file, used for Face Detection.
const char *faceCascadeFilename = "haar/haarcascade_frontalface_alt.xml";  // Haar face detector.
const char *eyeCascadeFilename1 = "haar/haarcascade_eye.xml";               // Basic eye detector for open eyes only.
const char *eyeCascadeFilename2 = "haar/haarcascade_eye_tree_eyeglasses.xml"; // Basic eye detector for open eyes if they might wear glasses.

// Set the desired face dimensions. Note that "getPreprocessedFace()" will return a square face.
const int faceWidth = 70;
const int faceHeight = faceWidth;

// Try to set the camera resolution. Note that this only works for some cameras on
// some computers and only for some drivers, so don't rely on it to work!
const int DESIRED_CAMERA_WIDTH = 640;
const int DESIRED_CAMERA_HEIGHT = 480;

// Parameters controlling how often to keep new faces when collecting them. Otherwise, the training set could look to similar to each other!
const double CHANGE_IN_IMAGE_FOR_COLLECTION = 0.3;      // How much the facial image should change before collecting a new face photo for training.
const double CHANGE_IN_SECONDS_FOR_COLLECTION = 1.0;    // How much time must pass before collecting a new face photo for training.

const char *windowName = "Webcam Face Recognition";   // Name shown in the GUI window.
const int BORDER = 8;  // Border between GUI elements to the edge of the image.

// Preprocess left & right sides of the face separately, in case there is stronger light on one side.
const bool preprocessLeftAndRightSeparately = true;

const string VOICE_AND_LEARNING = "JAVA";
const string ARDUINO = "ARD";
vector<string> tokens;

struct tm * time_prev;
struct tm * time_now;
int sec_prev;
int sec_now;
int min_prev;
int min_now;

struct clients
{
    int clientSocket_fileDescriptor;
    string id;
};

int cariSocketFDClient(struct clients myClients[], string desired_id){
    for (int i=0; i < MAX_CLIENTS; i++){
        if(myClients[i].id.compare(desired_id) == 0) return myClients[i].clientSocket_fileDescriptor;
    }
    return -1;
}

// Set to true if you want to see many windows created, showing various debug info. Set to 0 otherwise.
bool m_debug = 0;

// Running mode for the Webcam-based interactive GUI program.
enum MODES {MODE_STARTUP=0, MODE_DETECTION, MODE_COLLECT_FACES, MODE_TRAINING, MODE_RECOGNITION, MODE_DELETE_ALL, MODE_SEARCHING, MODE_TRAINING_SEARCH, MODE_REC_SEARCH, MODE_INIT, MODE_END};
const char* MODE_NAMES[] = {"Startup", "Detection", "Collect Faces", "Training", "Recognition", "Delete All", "Searching Database","Training Search", "Servo", "Init the Communication", "ERROR!"};
MODES m_mode = MODE_STARTUP;

int m_selectedPerson = -1;
int m_numPersons = 0;
vector<int> m_latestFaces;

// Position of GUI buttons:
Rect m_rcBtnAdd;
Rect m_rcBtnDel;
Rect m_rcBtnDebug;
int m_gui_faces_left = -1;
int m_gui_faces_top = -1;

// C++ conversion functions between integers (or floats) to std::string.
template <typename T> string toString(T t)
{
    ostringstream out;
    out << t;
    return out.str();
}

template <typename T> T fromString(string t)
{
    T out;
    istringstream in(t);
    in >> out;
    return out;
}


//fungsi buat dapetin ip addressnya client
void getClientAddress(char* outStr, struct sockaddr_in *addr){
    struct sockaddr_in* pV4Addr = addr;
    struct in_addr ipAddr = pV4Addr->sin_addr;
    inet_ntop( AF_INET, &ipAddr, outStr, INET_ADDRSTRLEN );
}

// Load the face and 1 or 2 eye detection XML classifiers.
void initDetectors(CascadeClassifier &faceCascade, CascadeClassifier &eyeCascade1, CascadeClassifier &eyeCascade2)
{
    // Load the Face Detection cascade classifier xml file.
    try {   // Surround the OpenCV call by a try/catch block so we can give a useful error message!
        faceCascade.load(faceCascadeFilename);
    } catch (cv::Exception &e) {}
    if ( faceCascade.empty() ) {
        cerr << "ERROR: Could not load Face Detection cascade classifier [" << faceCascadeFilename << "]!" << endl;
        cerr << "Copy your Face Detection cascade classifier into this WebcamFaceRec folder." << endl;
        exit(1);
    }
    cout << "Loaded the Face Detection cascade classifier [" << faceCascadeFilename << "]." << endl;

    // Load the Eye Detection cascade classifier xml file.
    try {   // Surround the OpenCV call by a try/catch block so we can give a useful error message!
        eyeCascade1.load(eyeCascadeFilename1);
    } catch (cv::Exception &e) {}
    if ( eyeCascade1.empty() ) {
        cerr << "ERROR: Could not load 1st Eye Detection cascade classifier [" << eyeCascadeFilename1 << "]!" << endl;
        cerr << "Copy your Eye Detection cascade classifier into this WebcamFaceRec folder." << endl;
        exit(1);
    }
    cout << "Loaded the 1st Eye Detection cascade classifier [" << eyeCascadeFilename1 << "]." << endl;

    // Load the Eye Detection cascade classifier xml file.
    try {   // Surround the OpenCV call by a try/catch block so we can give a useful error message!
        eyeCascade2.load(eyeCascadeFilename2);
    } catch (cv::Exception &e) {}
    if ( eyeCascade2.empty() ) {
        cerr << "Could not load 2nd Eye Detection cascade classifier [" << eyeCascadeFilename2 << "]." << endl;
        // Dont exit if the 2nd eye detector did not load, because we have the 1st eye detector at least.
        //exit(1);
    }
    else
        cout << "Loaded the 2nd Eye Detection cascade classifier [" << eyeCascadeFilename2 << "]." << endl;
}

// Get access to the webcam.
void initWebcam(VideoCapture &videoCapture, int cameraNumber)
{
    // Get access to the default camera.
    try {   // Surround the OpenCV call by a try/catch block so we can give a useful error message!
        videoCapture.open(cameraNumber);
    } catch (cv::Exception &e) {}
    if ( !videoCapture.isOpened() ) {
        cerr << "ERROR: Could not access the camera!" << endl;
        exit(1);
    }
    cout << "Loaded camera " << cameraNumber << "." << endl;
}


// Draw text into an image. Defaults to top-left-justified text, but you can give negative x coords for right-justified text,
// and/or negative y coords for bottom-justified text.
// Returns the bounding rect around the drawn text.
Rect drawString(Mat img, string text, Point coord, Scalar color, float fontScale = 0.6f, int thickness = 1, int fontFace = FONT_HERSHEY_COMPLEX)
{
    // Get the text size & baseline.
    int baseline=0;
    Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);
    baseline += thickness;

    // Adjust the coords for left/right-justified or top/bottom-justified.
    if (coord.y >= 0) {
        // Coordinates are for the top-left corner of the text from the top-left of the image, so move down by one row.
        coord.y += textSize.height;
    }
    else {
        // Coordinates are for the bottom-left corner of the text from the bottom-left of the image, so come up from the bottom.
        coord.y += img.rows - baseline + 1;
    }
    // Become right-justified if desired.
    if (coord.x < 0) {
        coord.x += img.cols - textSize.width + 1;
    }

    // Get the bounding box around the text.
    Rect boundingRect = Rect(coord.x, coord.y - textSize.height, textSize.width, baseline + textSize.height);

    // Draw anti-aliased text.
    putText(img, text, coord, fontFace, fontScale, color, thickness, CV_AA);

    // Let the user know how big their text is, in case they want to arrange things.
    return boundingRect;
}

// Draw a GUI button into the image, using drawString().
// Can specify a minWidth if you want several buttons to all have the same width.
// Returns the bounding rect around the drawn button, allowing you to position buttons next to each other.
Rect drawButton(Mat img, string text, Point coord, int minWidth = 0)
{
    int B = BORDER;
    Point textCoord = Point(coord.x + B, coord.y + B);
    // Get the bounding box around the text.
    Rect rcText = drawString(img, text, textCoord, CV_RGB(0,0,0));
    // Draw a filled rectangle around the text.
    Rect rcButton = Rect(rcText.x - B, rcText.y - B, rcText.width + 2*B, rcText.height + 2*B);
    // Set a minimum button width.
    if (rcButton.width < minWidth)
        rcButton.width = minWidth;
    // Make a semi-transparent white rectangle.
    Mat matButton = img(rcButton);
    matButton += CV_RGB(90, 90, 90);
    // Draw a non-transparent white border.
    rectangle(img, rcButton, CV_RGB(200,200,200), 1, CV_AA);

    // Draw the actual text that will be displayed, using anti-aliasing.
    drawString(img, text, textCoord, CV_RGB(10,55,20));

    return rcButton;
}

bool isPointInRect(const Point pt, const Rect rc)
{
    if (pt.x >= rc.x && pt.x <= (rc.x + rc.width - 1))
        if (pt.y >= rc.y && pt.y <= (rc.y + rc.height - 1))
            return true;

    return false;
}

// Mouse event handler. Called automatically by OpenCV when the user clicks in the GUI window.
void onMouse(int event, int x, int y, int, void*)
{
    // We only care about left-mouse clicks, not right-mouse clicks or mouse movement.
    if (event != CV_EVENT_LBUTTONDOWN)
        return;

    // Check if the user clicked on one of our GUI buttons.
    Point pt = Point(x,y);

    if (isPointInRect(pt, m_rcBtnAdd)) {
        cout << "User clicked [Add Person] button when numPersons was " << m_numPersons << endl;
        // Check if there is already a person without any collected faces, then use that person instead.
        // This can be checked by seeing if an image exists in their "latest collected face".
        if ((m_numPersons == 0) || (m_latestFaces[m_numPersons-1] >= 0)) {
            // Add a new person.
            m_numPersons++;
            m_latestFaces.push_back(-1); // Allocate space for an extra person.
            cout << "Num Persons: " << m_numPersons << endl;
        }
        
        // Use the newly added person. Also use the newest person even if that person was empty.
        m_selectedPerson = m_numPersons - 1;
        m_mode = MODE_COLLECT_FACES;
    }
    else if (isPointInRect(pt, m_rcBtnDebug)) {
        cout << "User clicked [Debug] button." << endl;
        m_debug = !m_debug;
        cout << "Debug mode: " << m_debug << endl;
    }

    else if (isPointInRect(pt, m_rcBtnDel)) {
        cout << "User clicked [Delete All] button." << endl;
        m_mode = MODE_DELETE_ALL;
    }

    else {
        cout << "User clicked on the image" << endl;
        
        // Check if the user clicked on one of the faces in the list.
        int clickedPerson = -1;
        for (int i=0; i<m_numPersons; i++) {
            if (m_gui_faces_top >= 0) {
                Rect rcFace = Rect(m_gui_faces_left, m_gui_faces_top + i * faceHeight, faceWidth, faceHeight);
                if (isPointInRect(pt, rcFace)) {
                    clickedPerson = i;
                    break;
                }
            }
        }

        // Change the selected person, if the user clicked on a face in the GUI.
        if (clickedPerson >= 0) {
            // Change the current person, and collect more photos for them.
            m_selectedPerson = clickedPerson; // Use the newly added person.
            m_mode = MODE_COLLECT_FACES;
        }
        
        // Otherwise they clicked in the center.
        else {
            // Change to training mode if it was collecting faces.
            if (m_mode == MODE_COLLECT_FACES) {
                cout << "User wants to begin training." << endl;
                m_mode = MODE_TRAINING;
            }
        }
    }
}

static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line, path, classlabel;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}


int main(int argc, char *argv[])
{
    CascadeClassifier faceCascade;
    CascadeClassifier eyeCascade1;
    CascadeClassifier eyeCascade2;
    VideoCapture videoCapture;

    cout << "Compiled with OpenCV version " << CV_VERSION << endl << endl;

    // Load the face and 1 or 2 eye detection XML classifiers.
    initDetectors(faceCascade, eyeCascade1, eyeCascade2);

    cout << endl;
    cout << "Hit 'Escape' in the GUI window to quit." << endl;

    // Allow the user to specify a camera number, since not all computers will be the same camera number.
    int cameraNumber = 0;   // Change this if you want to use a different camera device.
    if (argc > 1) {
        cameraNumber = atoi(argv[1]);
    }




    // Socket starts-------------------------------------------------------------------//
    /* LANGKAH 0: DEKLARASI VARIABLE */

    int opt = TRUE;

    int sockfd, newsockfd;  //file descriptor, posisi di array pointer, pointer2 ini nunjuk ke sebuah file disuatu tempat di memori
    int jumlah_byte;        //return value untuk fungsi read() sama write() nanti
    char buffer[1025];      //deklarasi besar ukuran buffer untuk pembacaan socket

    fd_set readfds;                         //deklarasi set socket description
    struct clients myClients[MAX_CLIENTS];  //variable yang dibutuhin sama fungsi select() (liat aja nanti dibawah)

    int currentClient_socketFD;         //array yang ngumpulin file descriptornya client2
    int highest_socketDescriptorNum;    //variable temporari buat nyimpen file descriptor kalo mau ngeloop sepanjang array client_sockets
    int activity;


    //struct sockaddr_in udah bawaan dari netinet/in.h, berikut definisinya
    //variable serv_addr akan berisi alamat dari server
    //variable cli_addr akan berisi alamat dari client yang terkoneksi ke server
    struct sockaddr_in address;
    int addrlen;
    
    //pesan welcome untuk client
    char message[] = "ECHO Daemon v1.0\n";
  
    //initialise all client_sockets[] to 0 so not checked
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        myClients[i].clientSocket_fileDescriptor = 0;
        myClients[i].id = "";
    }
      
    /* LANGKAH 1: DEKLARASI SOCKET */

    //buat socket baru, ada dua argument yang dibutuhkan untuk deklarasi socket
    //argument pertama, alamat domain dari socket: AF_INET (specify Internet Family IPv4) dan AF_UNIX (ga tau ini apaan)
    //argument kedua, tipe dari socket: SOCK_STREAM (specify transport layer protocol) dan SOCK_DGRAM (ga tau ini apaan)
    //argument ketiga, protokolnya: kalo 0 system yang bakal pilih protokol yang sesuai, defaultnya TCP 
    if( (sockfd = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
    {
        perror("ERROR establishing connection");
        exit(EXIT_FAILURE);
    }
    printf("[1] Socket file descriptor successfully created.\n");

    /* STEP 2: CREATE SOCKET CONFIGURATION */

    //setsockopt() untuk menspesifikasikan konfigurasi tertentu, disini kita mau konfigurasi waktu timeout
    //------------ hanya akan bekerja setelah client berhasil terkoneksi
    //------------ akan melakukan konfigurasi opsi socket yang ditentukan oleh:
    //argumen pertama, socket file descriptor
    //argumen kedua, level, untuk melakukan konfigurasi di tingkat socket, konstanta SOL_SOCKET digunakan
    //argumen ketiga, option_name, disini kita menggunakan SO_REUSEADDR (supaya bisa handle multiple client)
    //argumen keempat, option_value, berupa pointer ke value untuk ngubah opsinya
    //argumen kelima, option_len
    //--http://pubs.opengroup.org/onlinepubs/009695399/functions/setsockopt.html
    if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("ERROR configuring socket");
        exit(EXIT_FAILURE);
    }
    printf("[2] Socket SO_REUSEADDR config successfully configured.\n");

    /* LANGKAH 3: PANGGIL BIND */

    //inisialisasi socket structure
    address.sin_family = AF_INET;           //field ini berisi kode untuk address family
    address.sin_port = htons(PORT);         //buat port number, tapi lebih baik di convert dulu jadi network byte order
    address.sin_addr.s_addr = INADDR_ANY;   //field ini berisi IP address hostnya
                                            //--https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming
      
    //ngasih rincian yang ditentukan oleh serv_addr (termasuk informasi alamat dan port number) ke socket yang mau dipake
    //--newsockfd "ngebind" jadi sockfd dapat menggunakan alamat tersebut untuk melakukan koneksi ke newsockfd
    //----https://stackoverflow.com/questions/27014955/socket-connect-vs-bind
    //--address current host dan port number di server akan berjalan
    //----http://www.linuxhowtos.org/C_C++/socket.htm
    //argumen pertama, socket file descriptor
    //argumen kedua, pointer ke struct sockaddr tapi yang dilempar merupakan struct sockaddr_in
    //argumen ketiga, size dari address yang akan di bind
    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address))<0) 
    {
        perror("ERROR binding socket");
        exit(EXIT_FAILURE);
    }
    printf("[3] Socket successfully binded, listener on port %d.\n", PORT);
     
    /* LANGKAH 4: LISTEN */

    //mengijinkan proses untuk mendengarkan socket buat koneksi
    //argumen pertama, socket file descriptor
    //argumen kedua, ukuran "backlog-queue", banyaknya koneksi yang bisa menunggu selama proses melakkan handling koneksi tertentu
    //-------------- paling besar 5 yang diijinkan oleh kebanyakan sistem
    if (listen(sockfd, 3) < 0)
    {
        perror("ERROR listening connection");
        exit(EXIT_FAILURE);
    }
    printf("[4] 3 is set to maximum pending connections.\n");
      
    addrlen = sizeof(address);
    puts("[5] Waiting for connections\n...");
    // Socket ends-------------------------------------------------------------------//



    // Get access to the webcam.
    initWebcam(videoCapture, cameraNumber);

    // Try to set the camera resolution. Note that this only works for some cameras on
    // some computers and only for some drivers, so don't rely on it to work!
    videoCapture.set(CV_CAP_PROP_FRAME_WIDTH, DESIRED_CAMERA_WIDTH);
    videoCapture.set(CV_CAP_PROP_FRAME_HEIGHT, DESIRED_CAMERA_HEIGHT);

    // Create a GUI window for display on the screen.
    namedWindow(windowName, WINDOW_NORMAL); // Resizable window, might not work on Windows.
    resizeWindow(windowName, 640,480);
    moveWindow(windowName, 300,0);
    // Get OpenCV to automatically call my "onMouse()" function when the user clicks in the GUI window.
    setMouseCallback(windowName, onMouse, 0);

    // Run Face Recogintion interactively from the webcam. This function runs until the user quits.
    // recognizeAndTrainUsingWebcam(videoCapture, faceCascade, eyeCascade1, eyeCascade2);
    Ptr<FaceRecognizer> model;
    vector<Mat> preprocessedFaces;
    vector<Mat> initFaces;
    vector<int> faceLabels;
    vector<int> initLabels;
    vector<string> dirAllList;
    vector<string> dirSearchList;
    Mat old_prepreprocessedFace;
    double old_time = 0;
    int done = 0;
    clock_t t_prev;
    string bufferJava = "";
    string bufferArd = "";

    // Since we have already initialized everything, lets start in Detection mode.
    m_mode = MODE_INIT;
    // m_mode = MODE_DETECTION;
    // m_mode = MODE_SEARCHING;

    // Run forever, until the user hits Escape to "break" out of this loop.
    while (true) {
        // cout << "modennya adalah: " << m_mode << endl;

        ////////////////////////////////// SOCKETTTTTTTTT //////////////////////////////////////////////

        //clear the socket set
        FD_ZERO(&readfds);
  
        //clear socket set, parameter yang dibutuhkan adalah pointer ke variable bertipe data fd_set
        FD_SET(sockfd, &readfds);
        highest_socketDescriptorNum = sockfd;
         
        //ini loop buat masuk2in client file descriptor ke array client_sockets[]
        //dan sekaligus buat nyari yang punya nilai tertinggi
        for ( int i = 0 ; i < MAX_CLIENTS ; i++) 
        {
            //ambil socket descriptor yang mau diperiksa
            currentClient_socketFD = myClients[i].clientSocket_fileDescriptor;
             
            //hanya yang valid yang dimasukin (yang lebih dari 0 tandanya udah terdeclare file descriptornya)
            if(currentClient_socketFD > 0) FD_SET( currentClient_socketFD , &readfds);
             
            //angka file descriptor tertinggi + 1 nanti dibutuhkan sebagai parameter pertama fungsi select()
            //gua ga tau kenapa tapi emang gitu keterangannya --> nfds is the highest-numbered file descriptor in any of the three sets, plus 1.
            //--https://linux.die.net/man/2/select
            if(currentClient_socketFD > highest_socketDescriptorNum) highest_socketDescriptorNum = currentClient_socketFD;
        }

  
        //fungsi select() mengijinkan kita untuk memonitor beberapa file descriptor (socket), dan nunggu sampe ada salah satu file descriptor yang "aktif"
        //--------------- bekerja seperti interrupt handler, yang bisa aktif sesaat setelah file descriptor mendapatkan sebuah data
        //contoh jika tiba2 ada data yang bisa dibaca di salah satu socket, fungsi select() bakalan ngasih tau hal itu
        //argument pertama, nomor file descriptor tertinggi ditambah satu
        //argument kedua, socket set yang sudah kita persiapkan sejak awal
        //argument ketiga & keempat, dibiarin NULL aja, gua ga ngerti maksudnya apaan
        //argument kelima, timeout dalam bentuk struct timespec, kalo diisi NULL berarti nunggu terus sampe ada inputan masuk, berikut structnya
        //---struct timespec {
        //-----long tv_sec;
        //-----long tv_nsec;
        //---};
        struct timeval timeout;
        timeout.tv_sec = 0;  // 1 second timeout
        timeout.tv_usec = 100000; // 0 microseconds timeout, kalo ga di declare kadang2 suka geblek
        activity = select( highest_socketDescriptorNum + 1 , &readfds , NULL , NULL , &timeout);

        if (activity < 0) perror("ERROR when running select");
        else if (activity == 0) cout << "No activity. Do something else" << endl;

        //-----------------------------------------------------------------------------------------------------------------------------------------------//
        
        //kalo ada socket yang active di socket set kita (&readfds) maka process!
        if (FD_ISSET(sockfd, &readfds)) 
        {
            //accept() mengakibatkan proses diblok (ditahan) sampai client terkoneksi ke server
            //-------- melanjutkan kembali proses ketika koneksi dari client telah berhasil didirikan
            //-------- mengembalikan file descriptor baru! dan semua komunikasi dalam koneksi harus dilakukan menggunakan file deskirptor baru ini
            //argumen kedua, referensi pointer ke alamat client (ujung koneksi yang satunya)
            //argumen ketiga, ukuran dari strukturnya
            if ((newsockfd = accept(sockfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("\nNew connection! (socket fd: %d, ip: %s, port: %d).\n", newsockfd, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
              
            //karena udah berhasil konek sekarang kita simpen socket clientnya
            //inget karena clientnya bakalan lebih dari satu, kita simpen di array yang udah kita siapin tadi
            for (int i = 0; i < MAX_CLIENTS; i++) 
            {
                //kita puter (loop), kita cari yang masih kosong
                //kita sepakatnya kalo nilainya 0 berarti tandanya masih kosong, kalo udah lebih dari 0 berarti udah kepake ruangan arraynya
                if( myClients[i].clientSocket_fileDescriptor == 0 )
                {
                    //setelah ketemu yang kosong, kita masukin deh  
                    myClients[i].clientSocket_fileDescriptor = newsockfd;

                    //jangan lupa kasih tau user kalo kita udah berhasil nyimpen socket salah satu clientnya
                    printf("   adding to list of sockets in array [%d]\n\n", i);
                     
                    break;
                }
            }
        }

        ////////////////////////////////// SOCKETTTTTTTTT //////////////////////////////////////////////

        // For looping the m_selectedPerson
        int sp_loop = 0;
        
        string fn_csv = "/media/lintang/184060E54060CB58/Test-CV/train2/csvRyuji.ext";
        string fn_csv_search = "/media/lintang/184060E54060CB58/Test-CV/train2/csvSearch.ext";

        // Grab the next camera frame. Note that you can't modify camera frames.
        Mat cameraFrame;
        videoCapture >> cameraFrame;
        if( cameraFrame.empty() ) {
            cerr << "ERROR: Couldn't grab the next camera frame." << endl;
            exit(1);
        }

        // Get a copy of the camera frame that we can draw onto.
        Mat displayedFrame;
        cameraFrame.copyTo(displayedFrame);

        // Run the face recognition system on the camera image. It will draw some things onto the given image, so make sure it is not read-only memory!
        int identity = -1;

        // Find a face and preprocess it to have a standard size and contrast & brightness.
        Rect faceRect;  // Position of detected face.
        Rect searchedLeftEye, searchedRightEye; // top-left and top-right regions of the face, where eyes were searched.
        Point leftEye, rightEye;    // Position of the detected eyes.
        Mat preprocessedFace = getPreprocessedFace(displayedFrame, faceWidth, faceCascade, eyeCascade1, eyeCascade2, preprocessLeftAndRightSeparately, &faceRect, &leftEye, &rightEye, &searchedLeftEye, &searchedRightEye);

        bool gotFaceAndEyes = false;
        if (preprocessedFace.data)
            gotFaceAndEyes = true;

        // Draw an anti-aliased rectangle around the detected face.
        if (faceRect.width > 0) {
            rectangle(displayedFrame, faceRect, CV_RGB(255, 255, 0), 2, CV_AA);
        }

        if (m_mode == MODE_INIT) {

            // Buat di Personal
            // std::string filename = "ryuji.jar";
            // std::string command1 = "gnome-terminal --tab -e \"/bin/bash -c \'java -jar ryuji.jar; exec /bin/bash -i\'\"";
            // std::string pathJar = "/media/lintang/184060E54060CB58/Test-CV/train2/dist/";


            // // Buat di Ryuji
            // // std::string filename = "ryuji.jar";
            // // std::string command1 = "java -jar ";
            // // std::string pathJar = "/home/gunadarma/Documents/Ryuji_v1/RyujiFaceRec/dist/";            
 
            // // std::string commandfix = command1 + pathJar + filename;
            // std::string commandfix = command1;
            // system(commandfix.c_str());

            //yang diatas buat registrasi client aja, yang dibawah ini untuk operasi I/O  di socket2 yang kita punya :)
        for (int i = 0; i < MAX_CLIENTS; i++) 
        {
            //ambil socket descriptor yang mau diperiksa
            currentClient_socketFD = myClients[i].clientSocket_fileDescriptor;
              
            //kalo ada socket yang active di socket set kita (&readfds) maka process!
            if (FD_ISSET( currentClient_socketFD , &readfds)) 
            {
                //ambil informasi client yang mau nutup koneksi tersebut
                getpeername(currentClient_socketFD , (struct sockaddr*)&address , (socklen_t*)&addrlen);

                //kalo yang dibaca jumlah bytenya 0, itu tandanya ada client yang nutup koneksi
                if ((jumlah_byte = read( currentClient_socketFD , buffer, 1024)) == 0)
                {
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                      
                    //tutup soketnya
                    close( currentClient_socketFD );

                    //karena udah ditutup, array buat nyimpen file descriptornya harus dibalikin lagi jadi 0
                    //supaya ruangan arraynya bisa dipake lagi jadi tempat simpen file deskirptor yang baru
                    myClients[i].clientSocket_fileDescriptor = 0;
                }
                  
                //kalo yang dibaca jumlah bytenya lebih dari 0 berarti data yang kita pingin proses
                //pemrosesan data masuk dari soket koding disini!
                else
                {
                    //set the string terminating NULL byte on the end of the data read
                    buffer[jumlah_byte-1] = '\0';

                    //convert buffernya jadi string, soalnya lebih gampang proses dalam string
                    string buffer_str(buffer);
                    buffer_str = buffer_str.substr(0, buffer_str.length());

                    //parse data yang datang!
                    //ketentuan: setiap token dipisahin sama ';' token terakhir juga harus pake
                    //keluaran dari proses dibawah ini vector yang isinya token2 tersebut
                    vector<string> tokens;
                    size_t pos;
                    while ( (pos = buffer_str.find(';')) != std::string::npos)
                    {
                        string token = buffer_str.substr(0, pos);
                        tokens.push_back(token);
                        // cout << "test isi token: " << token << endl;
                        buffer_str.erase(0, pos + 1);
                    }

                    //yang valid itu kalo token yang terambil nilainya lebih dari dua
                    //jumlah token minimum yang dibutuhkan itu ketentuannya 2
                    if (tokens.size() >= 2 )
                    {
                        // cout << "ukuran token: " << tokens.size() << endl;
                        //////////////////////////////////////////////////////////////////////////////////////
                        //proses registrasi itu dibutuhin karena walaupun kita tau pesannya apa, dari mana ipnya, dan berapa portnya, kita ga tau siapa dia sebenernya
                        //maka dari itu kita tambahin informasi idnya ke array socket kita
                        if(tokens[0].compare("REG") == 0)
                        {
                            //ini berarti ada yang daftar dulu ke sini, kasih idnya dimasukkin ke array socket tadi
                            myClients[i].id = tokens[1];
                            // cout << "isi tokens: " << tokens[1] << endl;

                            //kirim message ke client kalo dirinya udah didaftarkan di server
                            char message[] = "REGD\n";
                            send(currentClient_socketFD , message , strlen(message) , 0 );
                            cout<<myClients[i].id<<"\t"<< myClients[i].clientSocket_fileDescriptor <<endl;
                            cout<< "SINI   "<<tokens[0]<<myClients[i].id<<endl;
                            // nambah counter
                            // sekaligus send ke 2 client "REGD;"

                            cout<< "nilai i: " << i << endl;
                            if(i == (MAX_CLIENTS-1)){
                                time_t t = time(0);   // get time now
                                time_prev = localtime( & t );
                                sec_prev = time_prev->tm_sec;
                                min_prev = time_prev->tm_min;
                                m_mode = MODE_RECOGNITION;
                            }
                        }
                    }

                }
            }
        }


        }
        else if (m_mode == MODE_DETECTION){
            // IDLE MODE

            // Loop untuk check socket ada command apa ga <----------------------------------------------------------------------
            // Sekalian receive name dari command find, kirim ke mode_searching
            for (int i = 0; i < MAX_CLIENTS; i++) {
            //ambil socket descriptor yang mau diperiksa
            currentClient_socketFD = myClients[i].clientSocket_fileDescriptor;
              
            //kalo ada socket yang active di socket set kita (&readfds) maka process!
            if (FD_ISSET( currentClient_socketFD , &readfds)) 
            {
                //ambil informasi client yang mau nutup koneksi tersebut
                getpeername(currentClient_socketFD , (struct sockaddr*)&address , (socklen_t*)&addrlen);

                //kalo yang dibaca jumlah bytenya 0, itu tandanya ada client yang nutup koneksi
                if ((jumlah_byte = read( currentClient_socketFD , buffer, 1024)) == 0)
                {
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                      
                    //tutup soketnya
                    close( currentClient_socketFD );

                    //karena udah ditutup, array buat nyimpen file descriptornya harus dibalikin lagi jadi 0
                    //supaya ruangan arraynya bisa dipake lagi jadi tempat simpen file deskirptor yang baru
                    myClients[i].clientSocket_fileDescriptor = 0;
                }
                  
                //kalo yang dibaca jumlah bytenya lebih dari 0 berarti data yang kita pingin proses
                //pemrosesan data masuk dari soket koding disini!
                else
                {
                    //set the string terminating NULL byte on the end of the data read
                    buffer[jumlah_byte-1] = '\0';

                    //convert buffernya jadi string, soalnya lebih gampang proses dalam string
                    string buffer_str(buffer);
                    buffer_str = buffer_str.substr(0, buffer_str.length());

                    //parse data yang datang!
                    //ketentuan: setiap token dipisahin sama ';' token terakhir juga harus pake
                    //keluaran dari proses dibawah ini vector yang isinya token2 tersebut
                    vector<string> tokens;
                    size_t pos;
                    while ( (pos = buffer_str.find(';')) != std::string::npos)
                    {
                        string token = buffer_str.substr(0, pos);
                        tokens.push_back(token);
                        buffer_str.erase(0, pos + 1);
                    }

                    //yang valid itu kalo token yang terambil nilainya lebih dari dua
                    //jumlah token minimum yang dibutuhkan itu ketentuannya 2
                    if (tokens.size() >= 2 )
                    {
                        //////////////////////////////////////////////////////////////////////////////////////
                        //proses registrasi itu dibutuhin karena walaupun kita tau pesannya apa, dari mana ipnya, dan berapa portnya, kita ga tau siapa dia sebenernya
                        //maka dari itu kita tambahin informasi idnya ke array socket kita
                        if(tokens[0].compare("REG") == 0)
                        {
                            //ini berarti ada yang daftar dulu ke sini, kasih idnya dimasukkin ke array socket tadi
                            myClients[i].id = tokens[1];

                            printf("JAVA | %s:%d > %s\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port), tokens[1].c_str());
                            //kirim message ke client kalo dirinya udah didaftarkan di server
                            char message[] = "REGD\n";
                            send(currentClient_socketFD , message , strlen(message) , 0 );
                            // nambah counter
                            // sekaligus send ke 2 client "REGD;"
                            // mode_recog
                        }


                        // IDLE

                        //kalo udah registrasi sebaiknya dari sisi client memberikan kirimian data ke socket dengan header/tanda "DATA"
                        //disini merupakan proses kita untuk baca-aca
                        else if (tokens[0].compare("DATA") == 0)
                        {
                            //sewaktu2 kalo kita butuh informasi siapa sih yang ngomong sekarang, mekanisme kayak gini enak bedainnya
                            //kalo yang dateng dari JAVA apa yang mau dilakuin... kalo yang dateng dari ARDUINO apa yang mau dilakuin....
                            //yang dibawah ini kalo yang ngomong itu dari si java
                            if (myClients[i].id.compare(VOICE_AND_LEARNING) == 0)
                            {
                                //print apa yang kita dapat
                                printf("JAVA | %s:%d > %s\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port), tokens[1].c_str());
                                bufferJava = tokens[1];
                                vector<string>().swap(dirSearchList);
                                m_mode = MODE_SEARCHING;

                                if (tokens[1].compare("EXIT") == 0){
                                    printf("JAVA | %s:%d > %s\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port), tokens[0].c_str());
                                    m_mode = MODE_RECOGNITION;
                                }
                            }

                            //yang dibawah in ikalo yang ngomong itu dari si arduino
                            else if (myClients[i].id.compare(ARDUINO) == 0)
                            {               
                                //print apa yang kita dapat
                                printf("ARDUINO | %s:%d > %s\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port), tokens[1].c_str());
                            }

                            //kirim peringatan
                            //char message[] = "Message received.\n";
                            //send(currentClient_socketFD , message , strlen(message) , 0 );
                        }

                        //kalo token awalnya ga dikenalin maka kasih tau kalo kita ga tau
                        else
                        {
                            //kirim peringatan
                            char message[] = "Tokens not identified. Try with REG or DATA.\n";
                            send(currentClient_socketFD , message , strlen(message) , 0 );
                        }
                    }

                    //kalo jumlah tokennya kurang dari 2 dianggap ga valid
                    else 
                    {
                        //kirim peringatan
                        char message[] = "Tokens are not valid.\n";

                        //cariSocketFDClient (myClients, "JAVA")
                        send(cariSocketFDClient(myClients, "JAVA") , message , strlen(message) , 0 );
                    }
                }
            }

        }
    }


        else if (m_mode == MODE_COLLECT_FACES) {

            // Check if we have detected a face.
            if (gotFaceAndEyes) {

                // Check if this face looks somewhat different from the previously collected face.
                double imageDiff = 10000000000.0;
                if (old_prepreprocessedFace.data) {
                    imageDiff = getSimilarity(preprocessedFace, old_prepreprocessedFace);
                }

                // Also record when it happened.
                double current_time = (double)getTickCount();
                double timeDiff_seconds = (current_time - old_time)/getTickFrequency();

                // Only process the face if it is noticeably different from the previous frame and there has been noticeable time gap.
                if ((imageDiff > CHANGE_IN_IMAGE_FOR_COLLECTION) && (timeDiff_seconds > CHANGE_IN_SECONDS_FOR_COLLECTION)) {
                    // Also add the mirror image to the training set, so we have more training data, as well as to deal with faces looking to the left or right.
                    Mat mirroredFace;
                    flip(preprocessedFace, mirroredFace, 1);

                    // Add the face images to the list of detected faces.
                    preprocessedFaces.push_back(preprocessedFace);
                    preprocessedFaces.push_back(mirroredFace);
                    faceLabels.push_back(m_selectedPerson);
                    faceLabels.push_back(m_selectedPerson);

                    cout << "length of array " << (preprocessedFaces.size()) << endl;

                    // Keep a reference to the latest face of each person.
                    m_latestFaces[m_selectedPerson] = preprocessedFaces.size() - 2;  // Point to the non-mirrored face.

                    // Show the number of collected faces. But since we also store mirrored faces, just show how many the user thinks they stored.
                    cout << "Saved face " << (preprocessedFaces.size()/2) << " for person " << m_selectedPerson << endl;

                    // Make a white flash on the face, so the user knows a photo has been taken.
                    Mat displayedFaceRegion = displayedFrame(faceRect);
                    displayedFaceRegion += CV_RGB(90,90,90);

                    // Keep a copy of the processed face, to compare on next iteration.
                    old_prepreprocessedFace = preprocessedFace;
                    old_time = current_time;

                    // Take database images, each person = 20 images // 40 in total with mirrored face
                    if (((preprocessedFaces.size()/2)%20 == 0)) {


                            bool loopSaved = 0;                      
                            // Saving to folder
                            string folderName = "";
                            string folderMain = "data";

                            // How to get a string/sentence with spaces
                            cout << "Please enter folder name:\n>";
                            getline(cin, folderName);
                            cout << "You entered: " << folderName << endl << endl;

                            string folderCreateCommand = "mkdir /media/lintang/184060E54060CB58/Test-CV/train2/data/" + folderName;
                            system(folderCreateCommand.c_str());
                            string folderCreateCommandSearching = "mkdir /media/lintang/184060E54060CB58/Test-CV/train2/data2/" + folderName;
                            system(folderCreateCommandSearching.c_str());

                        // ct < value, depends on how many images needed for database
                        for (int ct = 0; ct < 40 ; ct++){
                            stringstream ss;
                            string type = ".jpg";

                            ss<<"/media/lintang/184060E54060CB58/Test-CV/train2/data/"<<folderName<<"/"<<(ct + 1)<<type;

                            string fullPath = ss.str();
                            ss.str("");

                            loopSaved = imwrite(fullPath, preprocessedFaces[ct]);
                            // cout << "bool " << (loopSaved) << endl;
                        }
                            //Clear the array and start from 0 again
                        if (loopSaved == 1){
                            m_mode = MODE_TRAINING;
                        }

                        sp_loop = sp_loop++;
   
                    }
                }
            }
        }
        else if (m_mode == MODE_TRAINING) {

            std::string filename = "create_csv.py ";
            std::string command1 = "python ";
            std::string command2 = "/media/lintang/184060E54060CB58/Test-CV/train2/data/ > csvRyuji.ext";

            std::string commandfix = command1 + filename + command2;
            system(commandfix.c_str());
            
            DIR *d;
            struct dirent *dir;
            int i=0;
            d = opendir("/media/lintang/184060E54060CB58/Test-CV/train2/data/");
            if (d) {
                while ((dir = readdir(d)) != NULL) {
                    char a = dir->d_name[0];
                    if (a != '.') {
                        dirAllList.push_back(dir->d_name);
                    }
                }              
            }
            closedir(d);
                            
            // for(int i=0;i<dirAllList.size();i++) {
            //     cout<<dirAllList[i]<<endl; 
            // }
            
            bool haveEnoughData = true;

            // These vectors hold the images and corresponding labels:
            vector<Mat> images;
            vector<int> labels;
            // Read in the data (fails if no valid input filename is given, but you'll get an error message):
            try {
                read_csv(fn_csv, images, labels);
            } catch (cv::Exception& e) {
                cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
                // nothing more we can do
                exit(1);
            }

            if (haveEnoughData) {
                // Start training from the collected faces using Eigenfaces.
                model = learnCollectedFaces(images, labels, facerecAlgorithm);
                // Show the internal face recognition data, to help debugging.
                if (m_debug)
                    showTrainingDebugData(model, faceWidth, faceHeight);
                // // Now that training is over, we can start recognizing!
                m_mode = MODE_RECOGNITION;

            }
            else {
                // Since there isn't enough training data, go back to the face collection mode!
                m_mode = MODE_COLLECT_FACES;
            }

        }

        else if (m_mode == MODE_RECOGNITION) {
            if (model == NULL) {
                m_mode = MODE_TRAINING;
            }
            else {
                if (preprocessedFace.size().height == 0  ) {
                    char message[] = "NO1\n";
                    cout << "MASUKK SINI" <<endl;
                    //cariSocketFDClient (myClients, "JAVA")
                    send(cariSocketFDClient(myClients, "JAVA") , message , strlen(message) , 0 );
                    m_mode = MODE_DETECTION;
                }
                else {
                // Generate a face approximation by back-projecting the eigenvectors & eigenvalues.
                Mat reconstructedFace;
                reconstructedFace = reconstructFace(model, preprocessedFace);
                if (m_debug) {
                    if (reconstructedFace.data) {
                        imshow("reconstructedFace", reconstructedFace);
                    }
                }

                // Verify whether the reconstructed face looks like the preprocessed face, otherwise it is probably an unknown person.
                double similarity = getSimilarity(preprocessedFace, reconstructedFace);
                // cout << "nilai sim: "<<similarity << endl;
                string outputStr;
                if (similarity < 1e+08) {
                    // Identify who the person is in the preprocessed face image.
                    identity = model->predict(preprocessedFace);
                    outputStr = dirAllList[identity];
                    outputStr += "\n";   
                    
                    cout << "Identity: " << outputStr << ". Similarity: " << similarity << endl;
                    // cout<< clock() << "\t" << t_prev <<endl;
                    time_t t = time(0);
                    time_now = localtime( & t );
                    sec_now = time_now->tm_sec;
                    min_now = time_now->tm_min;

                    // if (((min_now * 60) + sec_now) - ((min_prev * 60) + sec_prev) <= 1){
                        // cout<<cariSocketFDClient(myClients, "JAVA")<<endl;
                        // cout<<"udah 3 detik"<<endl;
                        send(cariSocketFDClient(myClients, "JAVA") , outputStr.c_str() , strlen(outputStr.c_str()) , 0 );
                        m_mode = MODE_DETECTION;

                    // }

                }
                else {
                    // Since the confidence is low, assume it is an unknown person.
                    // outputStr = "USER NOT FOUND";
                    char message[] = "NO1\n";

                    //cariSocketFDClient (myClients, "JAVA")
                    send(cariSocketFDClient(myClients, "JAVA") , message , strlen(message) , 0 );
                    m_mode = MODE_DETECTION;
                }

                // cout << "Identity: " << outputStr << ". Similarity: " << similarity << endl;

                // ************************************* NEXT STEP AFTER DEMO ************************************
                // if (outputStr == "Unknown" && similarity >= 1e+08){
                //     // Check if there is already a person without any collected faces, then use that person instead.
                //     // This can be checked by seeing if an image exists in their "latest collected face".
                //     if ((m_numPersons == 0) || (m_latestFaces[m_numPersons-1] >= 0)) {
                //         // Add a new person.
                //         m_numPersons++;
                //         m_latestFaces.push_back(-1); // Allocate space for an extra person.
                //         cout << "Num Persons: " << m_numPersons << endl;
                //     }
        
                //     // Use the newly added person. Also use the newest person even if that person was empty.
                //     m_selectedPerson = m_numPersons - 1;
                    
                    // Confirm to add new person
                    // string answer = "";
                    // string answerYes = "Y";
                    // string answerNo = "N";
                    // cout << "Add New Person? (Y / N)";
                    // getline(cin, answer);
                    // if (answer == "Y" || answer == "y"){
                    //     m_mode = MODE_COLLECT_FACES;
                    // }
                    // else if (answer == "N" || answer == "n"){
                    //     m_mode = MODE_TRAINING;
                    // }
                    // else {
                    //     cout << "ERROR: Your answer can't be recognized by program.\n Program will EXIT" << endl;
                        // m_mode = MODE_RECOGNITION;
                    // }
                    
                // }
                // ***************************************** UNTIL HERE ************************************
            }
        }
        }

        else if (m_mode == MODE_SEARCHING) {
            // cout << bufferJava;
            // Receive bufferJava from mode_detection <--------------------------------------------------------------
            
            DIR *d;
            struct dirent *dir;
            vector<string> dirlist;
            int i=0;
            d = opendir("/media/lintang/184060E54060CB58/Test-CV/train2/data/");
            if (d) {
                while ((dir = readdir(d)) != NULL) {
                    dirlist.push_back(dir->d_name);
                }              
            }
            closedir(d);
            
            // // Dummy code to check input name
            
            // getline(cin, bufferJava);

            bool flag = 0;
            for(int i=0;i<dirlist.size();i++) {
                // cout<<dirlist[i]<<endl; 
                if (bufferJava == dirlist[i]){
                    // cout<<"DATA FOUND"<<endl;
                    flag = 1;
                    break;                    
                }    
            }

            if(flag == 1) {
                m_mode = MODE_TRAINING_SEARCH;
            }
            else{
                char message[] = "NO2\n";
                send(cariSocketFDClient(myClients, "JAVA") , message , strlen(message) , 0 );
                m_mode = MODE_DETECTION;
            }
            

        }

        else if (m_mode == MODE_TRAINING_SEARCH) {
            std::string filename = "create_csv_search.py ";
            std::string command1 = "python ";
            std::string command2 = "/media/lintang/184060E54060CB58/Test-CV/train2/data/";
            
            // Get string from java socket
            std::string bufferjavacommand = bufferJava;
            std::string command3 = " > csvSearch.ext";

            std::string commandfixSearch = command1 + filename + command2 + bufferjavacommand + command3;
            system(commandfixSearch.c_str());

            dirSearchList.push_back(bufferjavacommand);
            for(int i=0;i<dirSearchList.size();i++) {
                // cout<<dirSearchList[i]<<endl; 
            }
            

            bool haveEnoughData = true;
            // These vectors hold the images and corresponding labels:
            vector<Mat> images;
            vector<int> labels;
            // Read in the data (fails if no valid input filename is given, but you'll get an error message):
            try {
                read_csv(fn_csv_search, images, labels);
            } catch (cv::Exception& e) {
                cerr << "Error opening file \"" << fn_csv_search << "\". Reason: " << e.msg << endl;
                // nothing more we can do
                exit(1);
            }

            if (haveEnoughData) {
                // Start training from the collected faces using Eigenfaces.
                model = learnCollectedFaces(images, labels, facerecAlgorithm);
                // Show the internal face recognition data, to help debugging.
                if (m_debug)
                    showTrainingDebugData(model, faceWidth, faceHeight);
                // // Now that training is over, we can start recognizing!

                time_t t = time(0);   // get time now
                time_prev = localtime( & t );
                sec_prev = time_prev->tm_sec;
                min_prev = time_prev->tm_min;
                // Send signal to Servo to STOP
                char message[] = "c1\n";
                send(cariSocketFDClient(myClients, "ARD") , message , strlen(message) , 0 );
                m_mode = MODE_REC_SEARCH;
            }

        }

        else if (m_mode == MODE_REC_SEARCH){

            if (model == NULL) {
                m_mode = MODE_TRAINING_SEARCH;
            }

            else {
                // Generate a face approximation by back-projecting the eigenvectors & eigenvalues.
                Mat reconstructedFace;
                reconstructedFace = reconstructFace(model, preprocessedFace);
                if (m_debug)
                    if (reconstructedFace.data)
                        imshow("reconstructedFace", reconstructedFace);

                // Verify whether the reconstructed face looks like the preprocessed face, otherwise it is probably an unknown person.
                double similarity = getSimilarity(preprocessedFace, reconstructedFace);
                string outputStr;

                // cout<< clock() << "\t" << t_prev << endl;
                // if ( float ((clock() - t_prev)/CLOCKS_PER_SEC) <= 3.0) {
                time_t t = time(0);
                time_now = localtime( & t );
                sec_now = time_now->tm_sec;
                min_now = time_now->tm_min;

                if (((min_now * 60) + sec_now) - ((min_prev * 60) + sec_prev) <= 10){

                    // cout << "menunggu ..." << clock() << ", " << t_prev << ", " << clock()-t_prev << ", " << CLOCKS_PER_SEC << ", " << float ((clock() - t_prev)/CLOCKS_PER_SEC) << endl;
                    cout << "menunggu, " << sec_now << ", " << sec_prev << "\t" << difftime(sec_now, sec_prev) << endl;
                    if (similarity < UNKNOWN_PERSON_THRESHOLD) {
                    // Identify who the person is in the preprocessed face image.
                    identity = model->predict(preprocessedFace);
                    // outputStr = twaa010a0aaaaaaaaaaaaaaaoString(identity);
                    cout << dirSearchList.size() << ", " << identity << endl;
                    outputStr = dirSearchList[identity];

                    // cout<< outputStr.length() << "\t" << bufferJava.length() << endl;

                        if (outputStr == bufferJava){
                            // JANGAN DIHAPUS-----------------////////////////////
                            // Check if there is already a person without any collected faces, then use that person instead.
                            // This can be checked by seeing if an image exists in their "latest collected face".
                            if ((m_numPersons == 0) || (m_latestFaces[m_numPersons-1] >= 0)) {
                                // Add a new person.
                                m_numPersons++;
                                m_latestFaces.push_back(-1); // Allocate space for an extra person.
                                // cout << "Num Persons: " << m_numPersons << endl;
                            }
                
                            // Use the newly added person. Also use the newest person even if that person was empty.
                            m_selectedPerson = m_numPersons - 1;
                            // JANGAN DIHAPUS SAMPE SINI--------------------////////////////////
                            
                            // cout<<"COBAYAAA"<<endl;

                            outputStr += "\n";
                            cout<<outputStr<<endl;

                            int a = send(cariSocketFDClient(myClients, "JAVA") , outputStr.c_str() , strlen(outputStr.c_str()) , 0 );
                            // cout << "jawabnnya adalah: " << a << endl;
                            
                            // Send signal to Servo to STOP
                            char messagec21[] = "c2\n";
                            send(cariSocketFDClient(myClients, "ARD") , messagec21 , strlen(message) , 0 );
                            m_mode = MODE_DETECTION;
                        }
                    }   
                
                }
                else {    
                    // Since the confidence is low, assume it is an unknown person.
                    // outputStr = "USER NOT FOUND";
                    char message[] = "NO3\n";

                    //cariSocketFDClient (myClients, "JAVA")
                    send(cariSocketFDClient(myClients, "JAVA") , message , strlen(message) , 0 );
                    // Send signal to Servo to STOP
                    char messagec22[] = "c2\n";
                    send(cariSocketFDClient(myClients, "ARD") , messagec22 , strlen(message) , 0 );
                    m_mode = MODE_DETECTION;
                // cout << "Identity: " << outputStr << ". Similarity: " << similarity << endl;
                }
            }
        }

        else if (m_mode == MODE_DELETE_ALL) {
            // Restart everything!
            m_selectedPerson = -1;
            m_numPersons = 0;
            m_latestFaces.clear();
            preprocessedFaces.clear();
            faceLabels.clear();
            old_prepreprocessedFace = Mat();

            // Restart in Detection mode.
            m_mode = MODE_DETECTION;
        }
        else {
            cerr << "ERROR: Invalid run mode " << m_mode << endl;
            exit(1);
        }


        // Show the help, while also showing the number of collected faces. Since we also collect mirrored faces, we should just
        // tell the user how many faces they think we saved (ignoring the mirrored faces), hence divide by 2.
        string help;
        Rect rcHelp;
        if (m_mode == MODE_DETECTION)
            help = "Click [Add Person] when ready to collect faces.";
        else if (m_mode == MODE_COLLECT_FACES)
            help = "Click anywhere to train from your " + toString(preprocessedFaces.size()/2) + " faces of " + toString(m_numPersons) + " people.";
        else if (m_mode == MODE_TRAINING)
            help = "Please wait while your " + toString(preprocessedFaces.size()/2) + " faces of " + toString(m_numPersons) + " people builds.";
        else if (m_mode == MODE_RECOGNITION)
            help = "Click people on the right to add more faces to them, or [Add Person] for someone new.";
        if (help.length() > 0) {
            // Draw it with a black background and then again with a white foreground.
            // Since BORDER may be 0 and we need a negative position, subtract 2 from the border so it is always negative.
            float txtSize = 0.4;
            drawString(displayedFrame, help, Point(BORDER, -BORDER-2), CV_RGB(0,0,0), txtSize);  // Black shadow.
            rcHelp = drawString(displayedFrame, help, Point(BORDER+1, -BORDER-1), CV_RGB(255,255,255), txtSize);  // White text.
        }

        
        // Show the current mode.
        if (m_mode >= 0 && m_mode < MODE_END) {
            string modeStr = "MODE: " + string(MODE_NAMES[m_mode]);
            drawString(displayedFrame, modeStr, Point(BORDER, -BORDER-2 - rcHelp.height), CV_RGB(0,0,0));       // Black shadow
            drawString(displayedFrame, modeStr, Point(BORDER+1, -BORDER-1 - rcHelp.height), CV_RGB(0,255,0)); // Green text
        }

        // Show the current preprocessed face in the top-center of the display.
        int cx = (displayedFrame.cols - faceWidth) / 2;
        if (preprocessedFace.data) {
            // Get a BGR version of the face, since the output is BGR color.
            Mat srcBGR = Mat(preprocessedFace.size(), CV_8UC3);
            cvtColor(preprocessedFace, srcBGR, CV_GRAY2BGR);
            // Get the destination ROI (and make sure it is within the image!).
            //min(m_gui_faces_top + i * faceHeight, displayedFrame.rows - faceHeight);
            Rect dstRC = Rect(cx, BORDER, faceWidth, faceHeight);
            Mat dstROI = displayedFrame(dstRC);
            // Copy the pixels from src to dst.
            srcBGR.copyTo(dstROI);
        }
        // Draw an anti-aliased border around the face, even if it is not shown.
        rectangle(displayedFrame, Rect(cx-1, BORDER-1, faceWidth+2, faceHeight+2), CV_RGB(200,200,200), 1, CV_AA);

        // Draw the GUI buttons into the main image.
        // m_rcBtnAdd = drawButton(displayedFrame, "Add Person", Point(BORDER, BORDER));
        // m_rcBtnDel = drawButton(displayedFrame, "Delete All", Point(m_rcBtnAdd.x, m_rcBtnAdd.y + m_rcBtnAdd.height), m_rcBtnAdd.width);
        // m_rcBtnDebug = drawButton(displayedFrame, "Debug", Point(m_rcBtnDel.x, m_rcBtnDel.y + m_rcBtnDel.height), m_rcBtnDel.width);        

        // Show the most recent face for each of the collected people, on the right side of the display.
        m_gui_faces_left = displayedFrame.cols - BORDER - faceWidth;
        m_gui_faces_top = BORDER;
        for (int i=0; i<m_numPersons; i++) {
            int index = m_latestFaces[i];
            if (index >= 0 && index < (int)preprocessedFaces.size()) {
                Mat srcGray = preprocessedFaces[index];
                if (srcGray.data) {
                    // Get a BGR version of the face, since the output is BGR color.
                    Mat srcBGR = Mat(srcGray.size(), CV_8UC3);
                    cvtColor(srcGray, srcBGR, CV_GRAY2BGR);
                    // Get the destination ROI (and make sure it is within the image!).
                    int y = min(m_gui_faces_top + i * faceHeight, displayedFrame.rows - faceHeight);
                    Rect dstRC = Rect(m_gui_faces_left, y, faceWidth, faceHeight);
                    Mat dstROI = displayedFrame(dstRC);
                    // Copy the pixels from src to dst.
                    srcBGR.copyTo(dstROI);
                }
            }
        }

        // Highlight the person being collected, using a red rectangle around their face.
        if (m_mode == MODE_COLLECT_FACES) {
            if (m_selectedPerson >= 0 && m_selectedPerson < m_numPersons) {
                int y = min(m_gui_faces_top + m_selectedPerson * faceHeight, displayedFrame.rows - faceHeight);
                Rect rc = Rect(m_gui_faces_left, y, faceWidth, faceHeight);
                rectangle(displayedFrame, rc, CV_RGB(255,0,0), 3, CV_AA);
            }
        }

        // Highlight the person that has been recognized, using a green rectangle around their face.
        if (identity >= 0 && identity < 1000) {
            int y = min(m_gui_faces_top + identity * faceHeight, displayedFrame.rows - faceHeight);
            Rect rc = Rect(m_gui_faces_left, y, faceWidth, faceHeight);
            rectangle(displayedFrame, rc, CV_RGB(0,255,0), 3, CV_AA);
        }

        // Show the camera frame on the screen.
        imshow(windowName, displayedFrame);
        
        // If the user wants all the debug data, show it to them!
        if (m_debug) {
            Mat face;
            if (faceRect.width > 0) {
                face = cameraFrame(faceRect);
                if (searchedLeftEye.width > 0 && searchedRightEye.width > 0) {
                    Mat topLeftOfFace = face(searchedLeftEye);
                    Mat topRightOfFace = face(searchedRightEye);
                    imshow("topLeftOfFace", topLeftOfFace);
                    imshow("topRightOfFace", topRightOfFace);
                }
            }

            if (!model.empty())
                // // Start training from the collected faces using Eigenfaces.
                // model = learnCollectedFaces(images, labels, facerecAlgorithm);
                showTrainingDebugData(model, faceWidth, faceHeight);
        }

        
        // IMPORTANT: Wait for atleast 20 milliseconds, so that the image can be displayed on the screen!
        // Also checks if a key was pressed in the GUI window. Note that it should be a "char" to support Linux.
        char keypress = waitKey(20);  // This is needed if you want to see anything!

        if (keypress == VK_ESCAPE) {   // Escape Key
            // Quit the program!
            break;
        }

    }//end while
    return 0;
}
