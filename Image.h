//
// Created by agabor on 2017.09.13..
//

#ifndef CV_TEST_IMAGE_H
#define CV_TEST_IMAGE_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/nonfree/nonfree.hpp"
#include <opencv2/calib3d/calib3d.hpp>
#include <vector>
#include <map>
#include <tuple>

#define OCTAVES 4

struct MatchSettings {
    float scale0 = 4.0f;
    float scale1 = 3.0f;
    float scale2 = 3.0f;
    float scale3 = 2.0f;
    double contrast = 3.0f;
    double minHessian = 300.0f;
};

class Image {
public:
    explicit Image(const char* file_name);
    void resize();
    void scan(const MatchSettings &settings);
    void applyContrast(const MatchSettings &settings);
    std::tuple<int, cv::Mat> match(const Image &other, const MatchSettings &settings) const;


    cv::Mat original;
    cv::Mat mat;
    std::map<int,std::vector<cv::KeyPoint>> keypoints;
    std::map<int,cv::Mat> descriptors;
    std::map<int,std::vector<cv::KeyPoint>> scaled_keypoints;
    std::map<int,cv::Mat> scaled_descriptors;
    std::string file_name;

};


#endif //CV_TEST_IMAGE_H
