
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/nonfree/nonfree.hpp"
#include <opencv2/calib3d/calib3d.hpp>
#include "DicomReader.h"
#include <iostream>
#include <qt5/QtWidgets/QApplication>
#include "MainWindow.h"

#define OCTAVES 8

using namespace cv;
using namespace std;

void show(const Mat &image);




Mat match(Image &img1, Image &img2);

int main(int argc, char** argv)
{

    std::cout << "open cv " << CV_MAJOR_VERSION << '.' << CV_MINOR_VERSION << std::endl;
    Image img1("../test.dcm");
    Image img2("../test2.dcm");
    img2.resize(img1);
    cv::Mat dst;               // dst must be a different Mat
    cv::flip(img2.mat, dst, 1);
    img2.mat = dst;



    /*cv::SurfFeatureDetector detector{200,OCTAVES,1};

    img1.scan(detector);
    img2.scan(detector);
    Mat img_matches12 = match(img1, img2);*/
    QApplication app(argc, argv);
    auto *window = new MainWindow;

    window->init(QStringList() << "../test.dcm" << "../test2.dcm" << "../test3.dcm" << "../test4.dcm");

    window->show();

    return app.exec();
}

Mat match(Image &img1, Image &img2) {//-- Step 3: Matching descriptor vectors using FLANN matcher
    FlannBasedMatcher matcher;

    vector<KeyPoint> mkpoints1;
    vector<KeyPoint> mkpoints2;
    vector<DMatch> aggr_matches;

    for (int octave = OCTAVES; octave > 0; --octave) {

        auto it1 = img1.descriptors.find(octave);
        if (it1 == img1.descriptors.end())
            continue;

        auto descriptors1 = it1->second;


        auto it2 = img2.descriptors.find(octave);
        if (it2 == img2.descriptors.end())
            continue;

        auto descriptors2 = it2->second;

        vector<DMatch> matches;
        matcher.match(descriptors1, descriptors2, matches);
        vector<DMatch> matches_back;
        matcher.match(descriptors2, descriptors1, matches_back);
        vector<DMatch> matches_scaled;
        matcher.match(img1.scaled_descriptors[octave], img2.scaled_descriptors[octave], matches_scaled);


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
                mkpoints1.push_back(img1.keypoints[octave][match.queryIdx]);
                mkpoints2.push_back(img2.keypoints[octave][match.trainIdx]);
                match.queryIdx = static_cast<int>(mkpoints1.size() - 1);
                match.trainIdx = static_cast<int>(mkpoints2.size() - 1);
                aggr_matches.push_back(match);
                cout << match.distance << " " << matches_scaled[i].distance << endl;
            }
        }

    }


    Mat imk1, imk2;
    drawKeypoints(img1.mat, mkpoints1, imk1, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    drawKeypoints(img2.mat, mkpoints2, imk2, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    Mat img_matches;
    drawMatches(imk1, mkpoints1, imk2, mkpoints2,
                aggr_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    return img_matches;
}

