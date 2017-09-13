#include "Image.h"
#include "DicomReader.h"
#include <iostream>

using namespace cv;
using namespace std;



Image::Image(const char *file_name) {
    DicomReader r;
    r.addFile(file_name);
    r.config();
    mat= r.read(CV_8U)[0];
    Mat contr(mat.rows, mat.cols, mat.type());
    mat.convertTo(contr, -1, 3);
    mat = contr;
}

void Image::resize(const Image &templ)  {
    double m = sqrt(double(templ.mat.rows * templ.mat.cols) / double(mat.rows * mat.cols));
    cv::Mat dest(static_cast<int>(mat.rows * m), static_cast<int>(mat.cols * m), CV_8U);
    cv::resize(mat, dest, dest.size());
    mat = dest;
    //   cout <<  double(templ.mat.rows * templ.mat.cols) / double(mat.rows * mat.cols) << endl;
}


void addKeyPoint(const KeyPoint &k, map<int, vector<KeyPoint>> &kpmap) {
    if (kpmap.find(k.octave) == kpmap.end())
        kpmap[k.octave] = vector<KeyPoint>();
    kpmap[k.octave].push_back(k);
}

void Image::scan(cv::SurfFeatureDetector &detector) {
    vector<KeyPoint> points;
    detector.detect( mat, points );

    for(auto &k : points) {
        addKeyPoint(k, keypoints);
        KeyPoint scaled;
        scaled.size = k.size * (6.0d - k.octave);
        scaled.octave = k.octave;
        scaled.response = k.response;
        scaled.angle = k.angle;
        scaled.class_id = k.class_id;
        scaled.pt = k.pt;
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

tuple<int, Mat> Image::match(const Image &other) const {
    FlannBasedMatcher matcher;

    vector<KeyPoint> mkpoints1;
    vector<KeyPoint> mkpoints2;
    vector<DMatch> aggr_matches;

    for (int octave = OCTAVES; octave > 0; --octave) {

        auto it1 = descriptors.find(octave);
        if (it1 == descriptors.end())
            continue;

        auto descriptors1 = it1->second;


        auto it2 = other.descriptors.find(octave);
        if (it2 == other.descriptors.end())
            continue;

        auto descriptors2 = it2->second;

        vector<DMatch> matches;
        matcher.match(descriptors1, descriptors2, matches);
        vector<DMatch> matches_back;
        matcher.match(descriptors2, descriptors1, matches_back);
        vector<DMatch> matches_scaled;
        matcher.match(scaled_descriptors.at(octave), other.scaled_descriptors.at(octave), matches_scaled);


        map<int, int> bm;
        for (auto &m : matches_back) {
            bm[m.queryIdx] = m.trainIdx;
        }


        map<int, int> sm;
        for (auto &m : matches_scaled) {
            sm[m.queryIdx] = m.trainIdx;
        }


        for (int i = 0; i < matches.size(); ++i) {
            auto match = matches[i];
            if (bm[match.trainIdx] == match.queryIdx && sm[match.queryIdx] == match.trainIdx) {
                mkpoints1.push_back(keypoints.at(octave)[match.queryIdx]);
                mkpoints2.push_back(other.keypoints.at(octave)[match.trainIdx]);
                match.queryIdx = static_cast<int>(mkpoints1.size() - 1);
                match.trainIdx = static_cast<int>(mkpoints2.size() - 1);
                aggr_matches.push_back(match);
                cout << match.distance << " " << matches_scaled[i].distance << endl;
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
