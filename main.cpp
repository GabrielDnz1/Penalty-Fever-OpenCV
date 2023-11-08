#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <windows.h>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <mmsystem.h>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
using namespace std;
using namespace cv;

bool CheckCollision(const Point& circleCenter, const Rect& goleiroRect, int circleRadius) {
    Point goleiroCenter(goleiroRect.x + goleiroRect.width / 2, goleiroRect.y + goleiroRect.height / 2);

    int goleiroRadius = max(goleiroRect.width, goleiroRect.height) / 2;

    int distance = sqrt(pow(circleCenter.x - goleiroCenter.x, 2) + pow(circleCenter.y - goleiroCenter.y, 2));
    return distance < (circleRadius + goleiroRadius);
}

bool CheckGoalCollision(const Point& circleCenter, const Rect& rectangle, int circleRadius) {
    int positionXbola = circleCenter.x;
    int positionYbola = circleCenter.y;
    Point goleiroCenter(rectangle.x + rectangle.width / 2, rectangle.y + rectangle.height / 2);

    int rectangleRadius = max(rectangle.width, rectangle.height) / 2;

    int distance = sqrt(pow(circleCenter.x - goleiroCenter.x, 2) + pow(circleCenter.y - goleiroCenter.y, 2));
    return distance < (circleRadius + rectangleRadius);
}

int main() {
    int pontuacao = 0;
    int highscore = 0;

    game:
    bool back = false;
    string backgroundmusic = "C:/Users/diniz/trabalhodederzu/faceDetect/sounds/somdetorcida.wav";
    PlaySoundA(backgroundmusic.c_str(), NULL, SND_FILENAME | SND_ASYNC);
    
    string sadtrombone = "C:/Users/diniz/trabalhodederzu/faceDetect/sounds/sadtrombone.wav";
    string gol = "C:/Users/diniz/trabalhodederzu/faceDetect/sounds/gol.wav";

    int erroustatus = 0;
    int goalstatus = 0;

    Mat background = imread("C:/Users/diniz/trabalhodederzu/faceDetect/src/files/background.jpg");
    if (background.empty()) {
        cerr << "Não foi possível carregar a imagem de fundo." << endl;
        return -1;
    }

    Mat setaPower = imread("C:/Users/diniz/trabalhodederzu/faceDetect/src/files/SetaPower.png", IMREAD_UNCHANGED);
    if (setaPower.empty()) {
        cerr << "Não foi possível carregar a imagem da seta Power." << endl;
        return -1;
    }

    Mat goalkeeper = imread("C:/Users/diniz/trabalhodederzu/faceDetect/src/files/goleiro.png", IMREAD_UNCHANGED);
    if (goalkeeper.empty()) {
        cerr << "Não foi possível carregar a imagem do Goleiro" << endl;
        return -1;
    }

    int setaPowerWidth = setaPower.cols;
    int setaPowerHeight = setaPower.rows;

    int goalkeeperWidth = goalkeeper.cols;
    int goalkeeperHeight = goalkeeper.rows;

    int novaLarguraPower = 50;
    int novaAlturaPower = 50;

    int novaLarguraGoleiro = 100;
    int novaAlturaGoleiro = 100;

    resize(setaPower, setaPower, Size(novaLarguraPower, novaAlturaPower));
    resize(goalkeeper, goalkeeper, Size(novaLarguraGoleiro, novaAlturaGoleiro));

    int moveSpeedPower = 50;
    int positionXPower = background.cols * 0.25;
    int directionPower = 1;

    int verticalOffsetPower = 2;

    int moveSpeedCircle = 30;

    Point center(background.cols / 1.9, background.rows / 1.43);

    VideoCapture cap(1);
    if (!cap.isOpened()) {
        cerr << "Erro ao abrir a câmera." << endl;
        return -1;
    }

    CascadeClassifier eyeCascade;
    if (!eyeCascade.load("C:/Users/diniz/trabalhodederzu/faceDetect/src/haarcascade_frontalface_alt2.xml")) {
        cerr << "Não foi possível carregar o classificador haarcascade_eye.xml." << endl;
        return -1;
    }

    bool eyesDetected = false;
    bool gameStarted = false;
    bool colisao = false;
    bool colisaogoal = false;
    bool ballrelease = false;

    int posicaodabolaX = center.x;
    int posicaodabolaY = center.y;

    namedWindow("Penalty Fever");

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) {
            cerr << "Quadro vazio da câmera." << endl;
            break;
        }

        Mat image = background.clone();

        vector<Rect> eyes;
        eyeCascade.detectMultiScale(frame, eyes, 1.1, 2, 0, Size(30, 30));

        stringstream ss;
        ss << "Your Score: " << pontuacao << "  Highscore: " << highscore;
        putText(image, ss.str(), Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

        if (eyes.empty()) {
            eyesDetected = false;
            if (gameStarted) {
                moveSpeedPower = 0;
                if (center.x < positionXPower) {
                    center.x += moveSpeedCircle;
                } else if (center.x > positionXPower) {
                    center.x -= moveSpeedCircle;
                }
                if (center.y < (background.rows - setaPower.rows) / 5 + verticalOffsetPower) {
                    center.y += moveSpeedCircle;
                } else if (center.y > (background.rows - setaPower.rows) / 5 + verticalOffsetPower) {
                    center.y -= moveSpeedCircle;
                }
            }
        } else {
            eyesDetected = true;
            if (!gameStarted) {
                gameStarted = true;
                moveSpeedPower = 50;
            }
        }

        if (gameStarted) {
            int key = waitKey(30);
            if (key == 'q') {
                return 0;
            }
            positionXPower += directionPower * moveSpeedPower;
            int positionXGoleiro = frame.cols / 1.235 - novaLarguraGoleiro / 2;
            int positionYGoleiro = frame.rows / 1.5 - novaAlturaGoleiro;

            Rect goleirorect(positionXGoleiro, positionYGoleiro, goalkeeperWidth, goalkeeperHeight);
            int circleRadius = 10;

            int retanguloWidth = 140;
            int retanguloHeight = 150;
            int retanguloSpacing = 150;
            int retanguloY = 150;
            int retanguloX = 300 + (retanguloWidth + retanguloSpacing);
            Rect retangulo(retanguloX, retanguloY, retanguloWidth, retanguloHeight);            
            rectangle(image, retangulo, Scalar(0, 255, 0), 2);
            
            if (CheckCollision(center, goleirorect, circleRadius)) {
                colisao = true;
            } else {
                colisao = false;
            }
            if (CheckGoalCollision(center, retangulo, circleRadius)) {
                colisaogoal = true;
            } else {
                colisaogoal = false;
            }

            if (colisao == true && ballrelease == false) {        
                Mat penaltyPerdido = imread("C:/Users/diniz/trabalhodederzu/faceDetect/src/files/tialeila.jpg", IMREAD_UNCHANGED);
                PlaySoundA(sadtrombone.c_str(), NULL, SND_FILENAME | SND_ASYNC);
                if (!penaltyPerdido.empty()) {
                    imshow("Penalty Fever", penaltyPerdido);
                    int key = waitKey(0);
                    if (key == 'q') {
                    ofstream currentscore;
                    currentscore.open("C:/Users/diniz/trabalhodederzu/faceDetect/highscore.txt", ios::app);
                    if (currentscore.is_open()){
                        currentscore << pontuacao;
                        return 1;
                    }
                    ballrelease = true;               
                }
            }
        }

            if (colisaogoal == true && ballrelease == false) {     
                PlaySoundA(gol.c_str(), NULL, SND_FILENAME | SND_ASYNC);   
                pontuacao++;
                ballrelease = true;
                back = true;
                Sleep(5);
            }
            if(back == true){
                goto game;
            }

            if (positionXPower >= background.cols * 0.75 - setaPower.cols) {
                positionXPower = background.cols * 0.75 - setaPower.cols;
                directionPower = -1;
            } else if (positionXPower <= background.cols * 0.25) {
                positionXPower = background.cols * 0.25;
                directionPower = 1;
            }
        }

        for (int y = 0; y < setaPower.rows; y++) {
            for (int x = 0; x < setaPower.cols; x++) {
                Vec4b pixel = setaPower.at<Vec4b>(y, x);
                if (pixel[3] > 0) {
                    image.at<Vec3b>((background.rows - setaPower.rows) / 2 + verticalOffsetPower + y, positionXPower + x) = Vec3b(pixel[0], pixel[1], pixel[2]);
                }
            }
        }

        int positionXGoleiro = frame.cols / 1.235 - novaLarguraGoleiro / 2;
        int positionYGoleiro = frame.rows / 1.5 - novaAlturaGoleiro;
        for (int y = 0; y < novaAlturaGoleiro; y++) {
            for (int x = 0; x < novaLarguraGoleiro; x++) {
                Vec4b pixel = goalkeeper.at<Vec4b>(y, x);
                if (pixel[3] > 0) {
                    image.at<Vec3b>(positionYGoleiro + y, positionXGoleiro + x) = Vec3b(pixel[0], pixel[1], pixel[2]);
                }
            }
        }

        circle(image, center, 10, Scalar(255, 0, 0), -1);

        int cameraWidth = 200;
        int cameraHeight = 150;
        Mat cameraROI = image(Rect(background.cols - cameraWidth, background.rows - cameraHeight, cameraWidth, cameraHeight));
        resize(frame, cameraROI, Size(cameraWidth, cameraHeight));

        int retanguloWidth = 140;
        int retanguloHeight = 150;
        int retanguloSpacing = 150;
        int retanguloY = 150;

        for (int i = 0; i < 2; i++) {
            int retanguloX = 300 + (retanguloWidth + retanguloSpacing) * i;
            Rect retangulo(retanguloX, retanguloY, retanguloWidth, retanguloHeight);
            rectangle(image, retangulo, Scalar(0, 255, 0), 2);
        }
        Rect goleirorect(positionXGoleiro, positionYGoleiro, goalkeeperWidth, goalkeeperHeight);
        rectangle(image, goleirorect, Scalar(0, 255, 0), 2);

        imshow("Penalty Fever", image);

        int key = waitKey(30);
        if (key == 'q') {
            return 2;
        }
    }

    destroyAllWindows();

    cout << "Highscore: " << pontuacao << endl;
    return 0;
}
