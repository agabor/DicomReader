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

class Image {
public:
    explicit Image(const char* file_name);
    void resize();
    void scan();
    std::tuple<int, cv::Mat> match(const Image &other) const;

    cv::Mat mat;
    std::map<int,std::vector<cv::KeyPoint>> keypoints;
    std::map<int,cv::Mat> descriptors;
    std::map<int,std::vector<cv::KeyPoint>> scaled_keypoints;
    std::map<int,cv::Mat> scaled_descriptors;
};


#endif //CV_TEST_IMAGE_H
