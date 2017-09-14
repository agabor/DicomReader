//
// Created by agabor on 2017.09.14..
//

#include "MatchList.h"

MatchList::MatchList(const cv::Mat &descriptors1, const cv::Mat &descriptors2) {
    cv::BFMatcher matcher;
    matcher.match(descriptors1, descriptors2, *this, cv::Mat());
    fillEdges();
}

bool MatchList::has(int a, int b) const {
    auto it = edges.find(a);
    if (it == edges.end())
        return false;
    return it->second == b;
}

MatchList MatchList::filter(const MatchList &reverse) {
    MatchList result;
    for (auto &m : *this) {
        if (reverse.has(m.trainIdx, m.queryIdx))
            result.push_back(m);
    }
    result.fillEdges();
    return result;
}

void MatchList::fillEdges() {
    for (auto &m : *this) {
        edges[m.queryIdx] = m.trainIdx;
    }
}
