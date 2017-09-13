
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

Mat match(Image &img1, Image &img2);

int main(int argc, char** argv)
{

    std::cout << "open cv " << CV_MAJOR_VERSION << '.' << CV_MINOR_VERSION << std::endl;
    Image img1("../test.dcm");
    Image img2("../test2.dcm");
    img2.resize(img1);
    cv::Mat dst;               // dst must be a different Mat
    cv::flip(img2.mat, dst, 1);
    img2.mat = dst;



    /*cv::SurfFeatureDetector detector{200,OCTAVES,1};

    img1.scan(detector);
    img2.scan(detector);
    Mat img_matches12 = match(img1, img2);*/
    QApplication app(argc, argv);
    auto *window = new MainWindow;

    window->init(QStringList() << "../test.dcm" << "../test2.dcm" << "../test3.dcm" << "../test4.dcm");

    window->show();

    return app.exec();
}



