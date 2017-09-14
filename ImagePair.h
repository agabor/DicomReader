//
// Created by agabor on 2017.09.14..
//

#ifndef CV_TEST_IMAGEPAIR_H
#define CV_TEST_IMAGEPAIR_H


#include <memory>
#include <tuple>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "Image.h"

class ImagePair {
public:
ImagePair(std::shared_ptr<Image> a, std::shared_ptr<Image> b);
    std::tuple<int, cv::Mat> match(const MatchSettings &settings);
private:
    std::shared_ptr<Image> image_a;
    std::shared_ptr<Image> image_b;
    std::vector<cv::KeyPoint> matchedKeyPoints1;
    std::vector<cv::KeyPoint> matchedKeyPoints2;
    std::vector<cv::KeyPoint> matchedScaledKeyPoints1;
    std::vector<cv::KeyPoint> matchedScaledKeyPoints2;
    std::vector<cv::DMatch> aggr_matches;

    void matchOctave(const MatchSettings &settings, int octave);
};


#endif //CV_TEST_IMAGEPAIR_H
