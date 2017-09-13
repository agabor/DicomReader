
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



class Image {
public:
    explicit Image(const char* file_name){
        DicomReader r;
        r.addFile(file_name);
        r.config();
        mat= r.read(CV_8U)[0];
        Mat contr(mat.rows, mat.cols, mat.type());
        mat.convertTo(contr, -1, 3);
        mat = contr;
    }


        void resize(const Image &templ) {
        double m = sqrt(double(templ.mat.rows * templ.mat.cols) / double(mat.rows * mat.cols));
        Mat dest(static_cast<int>(mat.rows * m), static_cast<int>(mat.cols * m), CV_8U);
        cv::resize(mat, dest, dest.size());
        mat = dest;
           cout <<  double(templ.mat.rows * templ.mat.cols) / double(mat.rows * mat.cols) << endl;
    }

    void scan(cv::SurfFeatureDetector &detector){
        vector<KeyPoint> points;
        detector.detect( mat, points );

        for(auto &k : points) {
            addKeyPoint(k, keypoints);
            KeyPoint scaled;
            scaled.size = k.size * (5.0d - k.octave);
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

    void addKeyPoint(const KeyPoint &k, map<int, vector<KeyPoint>> &kpmap) const {
        if (kpmap.find(k.octave) == kpmap.end())
                kpmap[k.octave] = vector<KeyPoint>();
        kpmap[k.octave].push_back(k);
    }

    cv::Mat mat;
    map<int,vector<KeyPoint>> keypoints;
    map<int,Mat> descriptors;
    map<int,vector<KeyPoint>> scaled_keypoints;
    map<int,Mat> scaled_descriptors;
};
Mat match(Image &img1, Image &img2);

int main(int argc, char** argv)
{

    std::cout << "open cv " << CV_MAJOR_VERSION << '.' << CV_MINOR_VERSION << std::endl;
    Image img1("../test.dcm");
    Image img2("../test4.dcm");
    img2.resize(img1);
    cv::Mat dst;               // dst must be a different Mat
    cv::flip(img2.mat, dst, 1);
    img2.mat = dst;
    /*Image img3("../test3.dcm");
    img3.resize(img1);
    Image img4("../test4.dcm");
    img4.resize(img1);*/



    cv::SurfFeatureDetector detector{400,OCTAVES,1};

    img1.scan(detector);
    img2.scan(detector);
    //img3.scan(detector);
    //img4.scan(detector);
    Mat img_matches12 = match(img1, img2);
    //Mat img_matches13 = match(img1, img3);
    //Mat img_matches13r(img_matches12.rows, img_matches12.cols, img_matches12.type());
    //cv::resize(img_matches13, img_matches13r, img_matches13r.size());

    //Mat img_matches14 = match(img1, img4);
    //Mat img_matches14r(img_matches12.rows, img_matches12.cols, img_matches12.type());
    //cv::resize(img_matches14, img_matches14r, img_matches14r.size());

    //Mat img_matches;
    //vconcat(img_matches12, img_matches13r, img_matches);
    //vconcat(img_matches, img_matches14r, img_matches);

    QApplication app(argc, argv);
    //show(img_matches12);

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


        for (auto &match : matches) {
            if (match.distance <0.35 && bm[match.trainIdx] == match.queryIdx && sm[match.queryIdx] == match.trainIdx) {
                mkpoints1.push_back(img1.keypoints[octave][match.queryIdx]);
                mkpoints2.push_back(img2.keypoints[octave][match.trainIdx]);
                match.queryIdx = static_cast<int>(mkpoints1.size() - 1);
                match.trainIdx = static_cast<int>(mkpoints2.size() - 1);
                aggr_matches.push_back(match);
            }
        }

    }

    if (mkpoints1.size() > 3) {
        std::vector<Point2f> points1;
        std::vector<Point2f> points2;

        for (int i = 0; i < mkpoints1.size(); i++) {
            //-- Get the keypoints from the good matches
            points1.push_back(mkpoints1[i].pt);
            points2.push_back(mkpoints2[i].pt);
        }

        Mat H = findHomography(points1, points2, CV_LMEDS, 1);

        int count = 0;
        for (int i = 0; i < points1.size(); ++i) {
            if (H.data[i])
                ++count;
        }
        cout << "Homography: " << count << endl;
    }

    Mat imk1, imk2;
    drawKeypoints(img1.mat, mkpoints1, imk1, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    drawKeypoints(img2.mat, mkpoints2, imk2, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    Mat img_matches;
    drawMatches(imk1, mkpoints1, imk2, mkpoints2,
                aggr_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    cv::imshow("m", img_matches);
    cv::waitKey(0);
}

void show(const Mat &image) {
    auto *window = new MainWindow;

    window->setImage(image);

    window->show();
}