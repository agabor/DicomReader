
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/nonfree/nonfree.hpp"
#include <opencv2/calib3d/calib3d.hpp>
#include "DicomReader.h"
#include <iostream>
#include <qt5/QtWidgets/QApplication>
#include <qt5/QtWidgets/QMainWindow>
#include <qt5/QtWidgets/QLabel>
#include <qt5/QtWidgets/QLayout>
#include <qt5/QtWidgets/QVBoxLayout>
#include <qt5/QtGui/QImage>


using namespace cv;
using namespace std;

void show(const Mat &image);

int main(int argc, char** argv)
{

    std::cout << "open cv " << CV_MAJOR_VERSION << '.' << CV_MINOR_VERSION << std::endl;
    DicomReader r;
    r.addFile("../test.dcm");
    r.config();
    cv::Mat image = r.read(CV_8U)[0];

    int minHessian = 400;

    cv::SurfFeatureDetector detector( minHessian );

    std::vector<KeyPoint> keypoints_object, keypoints_scene;

    detector.detect( image, keypoints_object );

    cout << "keypoints: " << keypoints_object.size() << endl;


    //-- Draw keypoints
    Mat img_keypoints;

    drawKeypoints( image, keypoints_object, img_keypoints, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

    //cv::namedWindow("test");
    //cv::imshow("test", img_keypoints);
    //cv::waitKey(0);
    QApplication app(argc, argv);
    show(img_keypoints);

    return app.exec();
}

void show(const Mat &image) {
    QMainWindow *window = new QMainWindow;
    window->setMinimumSize(image.cols, image.rows);
    QLabel *label = new QLabel;
    label->setFixedSize(image.cols, image.rows);
    window->setLayout(new QVBoxLayout);
    window->layout()->addWidget(label);

    label->setPixmap(
            QPixmap::fromImage(QImage(image.data, image.cols, image.rows, int(image.step), QImage::Format_RGB888)));

    window->show();
}