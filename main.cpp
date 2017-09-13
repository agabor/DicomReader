
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
            if (keypoints.find(k.octave) == keypoints.end())
                keypoints[k.octave] = vector<KeyPoint>();
            keypoints[k.octave].push_back(k);
        }


        SurfDescriptorExtractor extractor;
        for (auto item : keypoints) {
            Mat desc;
            extractor.compute(mat, item.second, desc);
            descriptors[item.first] = desc;
        }
    }

    cv::Mat mat;
    map<int,vector<KeyPoint>> keypoints;
    map<int,Mat> descriptors;
};
Mat match(Image &img1, Image &img2);

int main(int argc, char** argv)
{

    std::cout << "open cv " << CV_MAJOR_VERSION << '.' << CV_MINOR_VERSION << std::endl;
    Image img1("../test.dcm");
    Image img2("../test2.dcm");
    img2.resize(img1);
    //cv::Mat dst;               // dst must be a different Mat
    //cv::flip(img2.mat, dst, 1);
    //img2.mat = dst;
    /*Image img3("../test3.dcm");
    img3.resize(img1);
    Image img4("../test4.dcm");
    img4.resize(img1);*/



    cv::SurfFeatureDetector detector{200,OCTAVES,1};

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

    for (int octave = OCTAVES; octave > 1; --octave) {

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


        map<int, int> bm;
        for (auto &m : matches_back) {
            bm[m.queryIdx] = m.trainIdx;
        }


        vector<DMatch> good_matches;
        for (auto &matche : matches) {
            if (bm[matche.trainIdx] == matche.queryIdx) {
                good_matches.push_back(matche);
            }
        }
        Mat imk1, imk2;
        drawKeypoints(img1.mat, img1.keypoints[octave], imk1, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        drawKeypoints(img2.mat, img2.keypoints[octave], imk2, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        Mat img_matches;
        drawMatches(imk1, img1.keypoints[octave], imk2, img2.keypoints[octave],
                    good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                    vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
        cv::imshow("m", img_matches);
        cv::waitKey(0);
    }
    //cout << "gm: " << good_matches.size() << endl;

    /*Mat img_matches;
    drawMatches(img1.mat, img1.keypoints, img2.mat, img2.keypoints,
                good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    return img_matches;*/
}

void show(const Mat &image) {
    auto *window = new MainWindow;

    window->setImage(image);

    window->show();
}