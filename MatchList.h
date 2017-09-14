//
// Created by agabor on 2017.09.14..
//

#ifndef CV_TEST_MATCHLIST_H
#define CV_TEST_MATCHLIST_H

#include <vector>
#include <map>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

class MatchList : public std::vector<cv::DMatch> {
public:
    MatchList(const cv::Mat &descriptors1, const cv::Mat &descriptors2);

    MatchList() = default;

    bool has(int a, int b) const ;

    MatchList filter(const MatchList &reverse);

private:

    void fillEdges();

    std::map<int, int> edges;
};


#endif //CV_TEST_MATCHLIST_H
