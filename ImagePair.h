//
// Created by agabor on 2017.09.14..
//

#ifndef CV_TEST_IMAGEPAIR_H
#define CV_TEST_IMAGEPAIR_H


#include <memory>
#include <tuple>

#include <opencv2/core/core.hpp>

#include "Image.h"

class ImagePair {
public:
ImagePair(std::shared_ptr<Image> a, std::shared_ptr<Image> b);
    std::tuple<int, cv::Mat> match(const MatchSettings &settings) const;
private:
    std::shared_ptr<Image> image_a;
    std::shared_ptr<Image> image_b;
};


#endif //CV_TEST_IMAGEPAIR_H
