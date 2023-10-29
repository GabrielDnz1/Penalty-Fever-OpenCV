#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

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

    Mat setaDirecao = imread("C:/Users/diniz/trabalhodederzu/faceDetect/src/files/Setadirecao.png", IMREAD_UNCHANGED);
    if (setaDirecao.empty()) {
        cerr << "Não foi possível carregar a imagem da seta Direcao." << endl;
        return -1;
    }

    int setaPowerWidth = setaPower.cols;
    int setaPowerHeight = setaPower.rows;
    int setaDirecaoWidth = setaDirecao.cols;
    int setaDirecaoHeight = setaDirecao.rows;

    int novaLarguraPower = 50;
    int novaAlturaPower = 50;
    int novaLarguraDirecao = 50;
    int novaAlturaDirecao = 50;

    resize(setaPower, setaPower, Size(novaLarguraPower, novaAlturaPower));
    resize(setaDirecao, setaDirecao, Size(novaLarguraDirecao, novaAlturaDirecao));

    int moveSpeed = 20;
    int positionXPower = 0;
    int positionXDirecao = background.cols - setaDirecao.cols;
    int directionPower = 1;
    int directionDirecao = -1;

    int verticalOffsetPower = 2;
    int verticalOffsetDirecao = 65;

    VideoCapture cap(1);
    if (!cap.isOpened()) {
        cerr << "Erro ao abrir a câmera." << endl;
        return -1;
    }

    CascadeClassifier eyeCascade;
    if (!eyeCascade.load("C:/Users/diniz/trabalhodederzu/faceDetect/src/haarcascade_eye.xml")) {
        cerr << "Não foi possível carregar o classificador haarcascade_eye.xml." << endl;
        return -1;
    }

    bool powerStopped = false;
    bool directionStopped = false;
    int blinkCounter = 0;

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) {
            cerr << "Quadro vazio da câmera." << endl;
            break;
        }

        Mat image = background.clone();

        // Detectar olhos na imagem da câmera
        vector<Rect> eyes;
        eyeCascade.detectMultiScale(frame, eyes, 1.1, 2, 0, Size(30, 30));

        for (const Rect& eye : eyes) {
            rectangle(frame, eye, Scalar(0, 0, 255), 2);
        }

        if (eyes.size() > 0) {
            blinkCounter++;
            if (blinkCounter % 2 == 1) {
                powerStopped = true;
                directionStopped = false;
            } else {
                powerStopped = false;
                directionStopped = true;
            }
        } else {
            blinkCounter = 0;
        }

        if (!powerStopped) {
            positionXPower += directionPower * moveSpeed;
            if (positionXPower >= background.cols - setaPower.cols) {
                positionXPower = background.cols - setaPower.cols;
                directionPower = -1;
            } else if (positionXPower <= 0) {
                positionXPower = 0;
                directionPower = 1;
            }
        }

        if (!directionStopped) {
            positionXDirecao += directionDirecao * moveSpeed;
            if (positionXDirecao >= background.cols - setaDirecao.cols) {
                positionXDirecao = background.cols - setaDirecao.cols;
                directionDirecao = -1;
            } else if (positionXDirecao <= 0) {
                positionXDirecao = 0;
                directionDirecao = 1;
            }
        }

        int posYPower = (background.rows - setaPower.rows) / 2 + verticalOffsetPower;
        int posYDirecao = (background.rows - setaDirecao.rows) / 2 + verticalOffsetDirecao;

        for (int y = 0; y < setaPower.rows; y++) {
            for (int x = 0; x < setaPower.cols; x++) {
                Vec4b pixel = setaPower.at<Vec4b>(y, x);
                if (pixel[3] > 0) {
                    image.at<Vec3b>(posYPower + y, positionXPower + x) = Vec3b(pixel[0], pixel[1], pixel[2]);
                }
            }
        }

        for (int y = 0; y < setaDirecao.rows; y++) {
            for (int x = 0; x < setaDirecao.cols; x++) {
                Vec4b pixel = setaDirecao.at<Vec4b>(y, x);
                if (pixel[3] > 0) {
                    image.at<Vec3b>(posYDirecao + y, positionXDirecao + x) = Vec3b(pixel[0], pixel[1], pixel[2]);
                }
            }
        }

        Point center(background.cols / 1.9, background.rows / 1.43);
        int radius = 10;
        Scalar color(255, 0, 0);
        int thickness = -1;

        circle(image, center, radius, color, thickness);

        putText(image, "Power", Point(positionXPower, posYPower + novaAlturaPower + 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);

        putText(image, "Direcao", Point(positionXDirecao, posYDirecao + novaAlturaDirecao + 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);

        int cameraWidth = 200;
        int cameraHeight = 150;
        Mat cameraROI = image(Rect(background.cols - cameraWidth, background.rows - cameraHeight, cameraWidth, cameraHeight));
        resize(frame, cameraROI, Size(cameraWidth, cameraHeight));

        imshow("Imagem com Círculo e Setas", image);

        int key = waitKey(30);

        if (key == 'q') {
            break;
        }
    }

    destroyAllWindows();
    return 0;
}