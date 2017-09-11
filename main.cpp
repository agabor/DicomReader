
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "DicomReader.h"
#include <iostream>
#include <qt5/QtWidgets/QApplication>
#include <qt5/QtWidgets/QMainWindow>
#include <qt5/QtWidgets/QLabel>
#include <qt5/QtWidgets/QLayout>
#include <qt5/QtWidgets/QVBoxLayout>
#include <qt5/QtGui/QImage>

int main(int argc, char** argv)
{

    std::cout << "open cv " << CV_MAJOR_VERSION << '.' << CV_MINOR_VERSION << std::endl;
    DicomReader r;
    r.addFile("../test.dcm");
    r.config();
    cv::Mat image = r.read(CV_8U)[0];

    cv::namedWindow("test");
    cv::imshow("test", image);
    //cv::waitKey(0);
    QApplication app(argc, argv);
    QMainWindow window;
    window.setMinimumSize(image.cols, image.rows);
    QLabel *label = new QLabel;
    label->setFixedSize(image.cols, image.rows);
    window.setLayout(new QVBoxLayout);
    window.layout()->addWidget(label);

    label->setPixmap(QPixmap::fromImage(QImage(image.data, image.cols, image.rows, int(image.step), QImage::Format_Grayscale8)));

    window.show();
    return app.exec();
}