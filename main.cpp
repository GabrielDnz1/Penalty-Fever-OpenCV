#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <windows.h>
#include <ctime>

using namespace std;
using namespace cv;

bool CheckCollision(const Point& center, const Point& goleiroCenter, int circleRadius, int goleiroRadius) {
    int distance = sqrt(pow(center.x - goleiroCenter.x, 2) + pow(center.y - goleiroCenter.y, 2));
    return distance < (circleRadius + goleiroRadius);
}

int main() {
    int highscore = 0; // Defina o highscore como 0

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

    Mat penaltyPerdido = imread("C:/Users/diniz/trabalhodederzu/faceDetect/src/files/penaltyperdido.jpg", IMREAD_UNCHANGED);
    if (penaltyPerdido.empty()) {
        cerr << "Não foi possível carregar a imagem 'penaltyperdido.gif'." << endl;
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

    int moveSpeedPower = 150;
    int positionXPower = background.cols * 0.25; // Inicie a seta a partir de 20% da tela
    int directionPower = 1;

    int verticalOffsetPower = 2;

    int moveSpeedCircle = 30; // Velocidade do círculo em direção à seta

    Point center(background.cols / 1.9, background.rows / 1.43); // Posição inicial do círculo

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
    bool gameStarted = false; // Variável para controlar o estado do jogo
    bool colisao = false; 
    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) {
            cerr << "Quadro vazio da câmera." << endl;
            break;
        }

        Mat image = background.clone();

        // Detectar retângulos de olhos na imagem da câmera
        vector<Rect> eyes;
        eyeCascade.detectMultiScale(frame, eyes, 1.1, 2, 0, Size(30, 30));

        stringstream ss;
        ss << "Highscore: " << highscore;
        putText(image, ss.str(), Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

        if (eyes.empty()) {
            eyesDetected = false;
            if (gameStarted) {
                moveSpeedPower = 0; // Pare a seta de direção quando não houver retângulos de olhos detectados
                if (center.x < positionXPower) {
                    center.x += moveSpeedCircle; // Move o círculo em direção à seta
                } else if (center.x > positionXPower) {
                    center.x -= moveSpeedCircle;
                }
                if (center.y < (background.rows - setaPower.rows) / 5 + verticalOffsetPower) {
                    center.y += moveSpeedCircle; // Move o círculo em direção à seta
                } else if (center.y > (background.rows - setaPower.rows) / 5 + verticalOffsetPower) {
                    center.y -= moveSpeedCircle;
                }
            }
        } else {
            eyesDetected = true;
            if (!gameStarted) {
                gameStarted = true;
                // Inicie o jogo, definindo a velocidade da seta
                moveSpeedPower = 100;
            }
        }

        if (gameStarted) {
            positionXPower += directionPower * moveSpeedPower;
            int positionXGoleiro = frame.cols / 1.235 - novaLarguraGoleiro / 2;
            int positionYGoleiro = frame.rows / 1.5 - novaAlturaGoleiro;

            if (positionXPower >= background.cols * 0.75 - setaPower.cols) { // Limite a posição da seta a 80% da tela
                positionXPower = background.cols * 0.75 - setaPower.cols;
                directionPower = -1;
                // Verificar a colisão entre o círculo e o goleiro
                Point goleiroCenter(positionXGoleiro + novaLarguraGoleiro / 2, positionYGoleiro + novaAlturaGoleiro / 2);
                int circleRadius = 10; // Raio do círculo
                int goleiroRadius = novaLarguraGoleiro / 15; 
                if (CheckCollision(center, goleiroCenter, circleRadius, goleiroRadius)) {
                    colisao = true;
                } else {
                    colisao = false;
                }

                // Exibir a imagem "penaltyperdido.gif" se houver colisão
                if (colisao) {
                    Mat penaltyPerdidoROI = image(Rect(center.x - penaltyPerdido.cols / 2, center.y - penaltyPerdido.rows / 2, penaltyPerdido.cols, penaltyPerdido.rows));
                    penaltyPerdido.copyTo(penaltyPerdidoROI);
                }

            } else if (positionXPower <= background.cols * 0.25) { // Limite a posição da seta a 20% da tela
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

        // retângulos
        int retanguloWidth = 138;
        int retanguloHeight = 150;
        int retanguloSpacing = 0;
        int retanguloY = 150;

        for (int i = 0; i < 3; i++) {
            int retanguloX = 300 + (retanguloWidth + retanguloSpacing) * i;
            Rect retangulo(retanguloX, retanguloY, retanguloWidth, retanguloHeight);
            rectangle(image, retangulo, Scalar(0, 255, 0), 2);
        }

        imshow("Penalty Fever", image);

        int key = waitKey(30);

        if (key == 'q') {
            break;
        }
    }

    destroyAllWindows();

    cout << "Highscore: " << highscore << endl; // Exibe o highscore no final do programa

    return 0;
}