#include "Image.h"
#include "DicomReader.h"
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;



Image::Image(const char *file_name) {
    DicomReader r;
    r.addFile(file_name);
    r.config();
    original = r.read(CV_8U)[0];
    resize();
}

void Image::applyContrast(const MatchSettings &settings) {
    mat = Mat(original.rows, original.cols, mat.type());
    original.convertTo(mat, -1, settings.contrast);
}

void Image::resize()  {
    double m = sqrt(double(400000) / double(original.rows * original.cols));
    cv::Mat dest(static_cast<int>(original.rows * m), static_cast<int>(original.cols * m), CV_8U);
    cv::resize(original, dest, dest.size());
    original = dest;
    //   cout <<  double(templ.mat.rows * templ.mat.cols) / double(mat.rows * mat.cols) << endl;
}


void addKeyPoint(const KeyPoint &k, map<int, vector<KeyPoint>> &kpmap) {
    if (kpmap.find(k.octave) == kpmap.end())
        kpmap[k.octave] = vector<KeyPoint>();
    kpmap[k.octave].push_back(k);
}



float getScale(int octave, const MatchSettings &settings) {
    switch (octave) {
        case 0: return settings.scale0;
        case 1: return settings.scale1;
        case 2: return settings.scale2;
        case 3: return settings.scale3;
        default: return settings.scale3;
    }
}

void Image::scan(const MatchSettings &settings) {
    delete mirrored;
    mirrored = nullptr;

    if (settings.mirrorY) {
        mirrored = new Image;
        mirrored->file_name = file_name;
        flip(original, mirrored->original, 1);
        mirrored->scanSelf(settings);
    }

    scanSelf(settings);
}

void Image::scanSelf(const MatchSettings &settings) {
    applyContrast(settings);
    SurfFeatureDetector detector{settings.minHessian, OCTAVES, 1};

    keypoints.clear();
    descriptors.clear();
    scaled_keypoints.clear();
    scaled_descriptors.clear();

    vector<KeyPoint> points;
    detector.detect(mat, points );

    sort(points.begin(), points.end(),
         [](const KeyPoint & a, const KeyPoint & b) -> bool
         {
             return a.response > b.response;
         });

    for(auto &k : points) {
        addKeyPoint(k, keypoints);
        KeyPoint scaled;
        scaled = getScaledKeyPoint(settings, k, scaled);
        addKeyPoint(scaled, scaled_keypoints);
    }


    SurfDescriptorExtractor extractor;
    for (auto item : keypoints) {
        Mat desc;
        extractor.compute(mat, item.second, desc);
        descriptors[item.first] = desc;
    }
    for (auto item : scaled_keypoints) {
        Mat desc;
        extractor.compute(mat, item.second, desc);
        scaled_descriptors[item.first] = desc;
    }
}

KeyPoint &Image::getScaledKeyPoint(const MatchSettings &settings, const KeyPoint &k, KeyPoint &scaled) const {
    scaled.size = k.size * getScale(k.octave, settings);
    scaled.octave = k.octave;
    scaled.response = k.response;
    scaled.angle = k.angle;
    scaled.class_id = k.class_id;
    scaled.pt = k.pt;
    return scaled;
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

    float getDist(int a, int b) {
        for (auto &m : *this) {
            if (m.queryIdx == a && m.trainIdx == b)
                return m.distance;
        }
        return -1.0f;
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

tuple<int, Mat> Image::match(const Image &other, const MatchSettings &settings) const {

    vector<KeyPoint> mkpoints1;
    vector<KeyPoint> mkpoints2;
    vector<DMatch> aggr_matches;

    for (int octave = OCTAVES; octave >= 0; --octave) {

        auto it1 = descriptors.find(octave);
        if (it1 == descriptors.end())
            continue;

        auto descriptors1 = it1->second;


        auto it2 = other.descriptors.find(octave);
        if (it2 == other.descriptors.end())
            continue;

        auto descriptors2 = it2->second;

        MatchList matches = getMatchList(descriptors1, descriptors2, settings);
        MatchList matches_scaled;
        if (settings.scale) {
            matches_scaled = getMatchList(scaled_descriptors.at(octave),
                                                         other.scaled_descriptors.at(octave), settings);
        }

        for (size_t i = 0; i < matches.size(); ++i) {
            auto match = matches[i];
            if (!settings.scale || matches_scaled.has(match.queryIdx, match.trainIdx)) {
                mkpoints1.push_back(keypoints.at(octave)[match.queryIdx]);
                mkpoints2.push_back(other.keypoints.at(octave)[match.trainIdx]);
                match.queryIdx = static_cast<int>(mkpoints1.size() - 1);
                match.trainIdx = static_cast<int>(mkpoints2.size() - 1);
                aggr_matches.push_back(match);
            }
        }

    }


    Mat imk1, imk2;
    drawKeypoints(mat, mkpoints1, imk1, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    drawKeypoints(other.mat, mkpoints2, imk2, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    Mat img_matches;
    drawMatches(imk1, mkpoints1, imk2, mkpoints2,
                aggr_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    return tuple<int, Mat>(aggr_matches.size(), img_matches);
}
