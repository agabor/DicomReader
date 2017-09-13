
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/nonfree/nonfree.hpp"
#include <opencv2/calib3d/calib3d.hpp>
#include "DicomReader.h"
#include <iostream>
#include <qt5/QtWidgets/QApplication>
#include "MainWindow.h"


using namespace cv;
using namespace std;


int main(int argc, char** argv)
{

    std::cout << "open cv " << CV_MAJOR_VERSION << '.' << CV_MINOR_VERSION << std::endl;

    QApplication app(argc, argv);
    auto *window = new MainWindow;

    window->init(QStringList() << "../test.dcm" << "../test2.dcm" << "../test3.dcm" << "../test4.dcm");

    window->show();

    return app.exec();
}



