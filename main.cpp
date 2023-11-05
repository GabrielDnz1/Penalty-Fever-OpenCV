#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <windows.h>

using namespace std;
using namespace cv;

int main() {
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

    int moveSpeedPower = 100;
    int positionXPower = 0;
    int directionPower = 1;

    int verticalOffsetPower = 2;

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

        for (const Rect& eye : eyes) {
            rectangle(frame, eye, Scalar(0, 0, 255), 2);
        }

        if (eyes.empty()) {
            eyesDetected = false;
            moveSpeedPower = 0; // Parar a seta de direção quando não houver retângulos de olhos detectados
            Sleep(10);
        } else {
            eyesDetected = true;
            moveSpeedPower = 100; // Restaurar a velocidade da seta de direção se olhos forem detectados
        }

        positionXPower += directionPower * moveSpeedPower;

        if (positionXPower >= background.cols - setaPower.cols) {
            positionXPower = background.cols - setaPower.cols;
            directionPower = -1;
        } else if (positionXPower <= 0) {
            positionXPower = 0;
            directionPower = 1;
        }

        int posYPower = (background.rows - setaPower.rows) / 2 + verticalOffsetPower;

        for (int y = 0; y < setaPower.rows; y++) {
            for (int x = 0; x < setaPower.cols; x++) {
                Vec4b pixel = setaPower.at<Vec4b>(y, x);
                if (pixel[3] > 0) {
                    image.at<Vec3b>(posYPower + y, positionXPower + x) = Vec3b(pixel[0], pixel[1], pixel[2]);
                }
            }
        }

        // Sempre adicione a imagem do goleiro
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

        Point center(background.cols / 1.9, background.rows / 1.43);
        int radius = 10;
        Scalar color(255, 0, 0);
        int thickness = -1;

        circle(image, center, radius, color, thickness);

        int cameraWidth = 200;
        int cameraHeight = 150;
        Mat cameraROI = image(Rect(background.cols - cameraWidth, background.rows - cameraHeight, cameraWidth, cameraHeight));
        resize(frame, cameraROI, Size(cameraWidth, cameraHeight));

        // retangulos
        int retanguloWidth = 138;
        int retanguloHeight = 150;
        int retanguloSpacing = 0;
        int retanguloY = 150;

        for (int i = 0; i < 3; i++) {
            int retanguloX = 300 + (retanguloWidth + retanguloSpacing) * i;
            Rect retangulo(retanguloX, retanguloY, retanguloWidth, retanguloHeight);
            rectangle(image, retangulo, Scalar(0, 255, 0), 2);
        }

        imshow("Imagem com Círculo, Seta de Power e Goleiro", image);

        int key = waitKey(30);

        if (key == 'q') {
            break;
        }
    }

    destroyAllWindows();
    return 0;
}
