
#include <map>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include "ImagePair.h"


using namespace cv;
using namespace std;

ImagePair::ImagePair(std::shared_ptr<Image> a, std::shared_ptr<Image> b) {
    image_a = std::move(a);
    image_b = std::move(b);
}

class MatchList : public vector<DMatch> {
public:
    MatchList(const Mat &descriptors1, const Mat &descriptors2) {
        BFMatcher matcher;
        matcher.match(descriptors1, descriptors2, *this, Mat());
        fillEdges();
    }

    MatchList() = default;

    bool has(int a, int b) const {
        auto it = edges.find(a);
        if (it == edges.end())
            return false;
        return it->second == b;
    }

    MatchList filter(const MatchList &reverse){
        MatchList result;
        for (auto &m : *this) {
            if (reverse.has(m.trainIdx, m.queryIdx))
                result.push_back(m);
        }
        result.fillEdges();
        return result;
    }

private:

    void fillEdges() {
        for (auto &m : *this) {
            edges[m.queryIdx] = m.trainIdx;
        }
    }

    map<int, int> edges;
};


MatchList getReverseFilteredMatchList(const Mat &descriptors1, const Mat &descriptors2) {
    MatchList matches(descriptors1, descriptors2);
    MatchList matches_back(descriptors2, descriptors1);
    return matches.filter(matches_back);
}

MatchList getMatchList(const Mat &descriptors1, const Mat &descriptors2, const MatchSettings &settings) {
    if (settings.reverse)
        return getReverseFilteredMatchList(descriptors1, descriptors2);
    else
        return MatchList(descriptors1, descriptors2);
}

Mat drawKeypoints(const vector<KeyPoint> &keypoints, Mat &image)  {
    Mat imk1;
    cv::drawKeypoints(image, keypoints, imk1, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    return imk1;
}


tuple<int, Mat> ImagePair::match(const MatchSettings &settings) const {

    vector<KeyPoint> matchedKeyPoints1;
    vector<KeyPoint> matchedKeyPoints2;
    vector<KeyPoint> matchedScaledKeyPoints1;
    vector<KeyPoint> matchedScaledKeyPoints2;
    vector<DMatch> aggr_matches;

    for (int octave = OCTAVES; octave >= 0; --octave) {

        auto it1 = image_a->descriptors.find(octave);
        if (it1 == image_a->descriptors.end())
            continue;

        auto descriptors1 = it1->second;


        auto it2 = image_b->descriptors.find(octave);
        if (it2 == image_b->descriptors.end())
            continue;

        auto descriptors2 = it2->second;

        MatchList matches = getMatchList(descriptors1, descriptors2, settings);
        MatchList matches_scaled;
        if (settings.scale) {
            matches_scaled = getMatchList(image_a->scaled_descriptors.at(octave),
                                          image_b->scaled_descriptors.at(octave), settings);
        }

        for (size_t i = 0; i < matches.size(); ++i) {
            auto match = matches[i];
            if (!settings.scale || matches_scaled.has(match.queryIdx, match.trainIdx)) {
                matchedKeyPoints1.push_back(image_a->keypoints.at(octave)[match.queryIdx]);
                matchedKeyPoints2.push_back(image_b->keypoints.at(octave)[match.trainIdx]);
                match.queryIdx = static_cast<int>(matchedKeyPoints1.size() - 1);
                match.trainIdx = static_cast<int>(matchedKeyPoints2.size() - 1);
                aggr_matches.push_back(match);
                if (settings.scale) {
                    matchedScaledKeyPoints1.push_back(image_a->scaled_keypoints.at(octave)[match.queryIdx]);
                    matchedScaledKeyPoints2.push_back(image_b->scaled_keypoints.at(octave)[match.trainIdx]);
                }
            }
        }

    }


    Mat imk1 = drawKeypoints(matchedKeyPoints1, image_a->mat);
    Mat imk2 = drawKeypoints(matchedKeyPoints1, image_b->mat);

    if (settings.scale) {
        imk1 = drawKeypoints(matchedScaledKeyPoints1, imk1);
        imk2 = drawKeypoints(matchedScaledKeyPoints1, imk2);
    }

    Mat img_matches;
    cv::drawMatches(imk1, matchedKeyPoints1, imk2, matchedKeyPoints2,
                    aggr_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                    vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    return tuple<int, Mat>(aggr_matches.size(), img_matches);
}