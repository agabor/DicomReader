
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "DicomReader.h"
#include <iostream>

int main() {

    std::cout << "open cv " << CV_MAJOR_VERSION << '.' << CV_MINOR_VERSION << std::endl;
    DicomReader r;
    r.addFile("../test.dcm");
    r.config();
    cv::Mat image = r.read(CV_64F)[0];

    cv::namedWindow("test");
    cv::imshow("test", image);
    cv::waitKey(0);
    return 0;
}