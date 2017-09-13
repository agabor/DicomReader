
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

    }


        void resize(const Image &templ) {
        double m = sqrt(double(templ.mat.rows * templ.mat.cols) / double(mat.rows * mat.cols));
        Mat dest(static_cast<int>(mat.rows * m), static_cast<int>(mat.cols * m), CV_8U);
        cv::resize(mat, dest, dest.size());
        mat = dest;
           cout <<  double(templ.mat.rows * templ.mat.cols) / double(mat.rows * mat.cols) << endl;
    }

    void scan(cv::SurfFeatureDetector &detector){
        detector.detect( mat, keypoints );

        double avg = 0;
        float min = 100000;
        float max = 0;

        double ravg = 0;
        float rmin = 100000;
        float rmax = 0;

        sort(keypoints.begin(), keypoints.end(),
             [](const KeyPoint & a, const KeyPoint & b) -> bool
             {
                 return a.response > b.response;
             });

        //keypoints.resize(20);

        map<int, int> h;
        for(auto &k : keypoints) {
            if (h.find(k.size) == h.end())
                h[k.size] = 1;
            else
                h[k.size] += 1;
        }

        for (auto &i : h) {
            cout << "(" << i.first << ", " << i.second << ")" << endl;
        }

        cout << endl;
        cout << endl;

        SurfDescriptorExtractor extractor;
        extractor.compute( mat, keypoints, descriptors );
    }

    cv::Mat mat;
    std::vector<KeyPoint> keypoints;
    cv::Mat descriptors;
};
Mat match(const Image &img1, const Image &img2);

int main(int argc, char** argv)
{

    std::cout << "open cv " << CV_MAJOR_VERSION << '.' << CV_MINOR_VERSION << std::endl;
    Image img1("../test.dcm");
    Image img2("../test2.dcm");
    img2.resize(img1);
    //cv::Mat dst;               // dst must be a different Mat
    //cv::flip(img2.mat, dst, 1);
    //img2.mat = dst;
    Image img3("../test3.dcm");
    img3.resize(img1);
    Image img4("../test4.dcm");
    img4.resize(img1);



    cv::SurfFeatureDetector detector{100,16,8};

    img1.scan(detector);
    img2.scan(detector);
    img3.scan(detector);
    img4.scan(detector);
    Mat img_matches12 = match(img1, img2);
    Mat img_matches13 = match(img1, img3);
    Mat img_matches13r(img_matches12.rows, img_matches12.cols, img_matches12.type());
    cv::resize(img_matches13, img_matches13r, img_matches13r.size());

    Mat img_matches14 = match(img1, img4);
    Mat img_matches14r(img_matches12.rows, img_matches12.cols, img_matches12.type());
    cv::resize(img_matches14, img_matches14r, img_matches14r.size());

    Mat img_matches;
    vconcat(img_matches12, img_matches13r, img_matches);
    vconcat(img_matches, img_matches14r, img_matches);

    QApplication app(argc, argv);
    show(img_matches);

    return app.exec();
}

Mat match(const Image &img1, const Image &img2) {//-- Step 3: Matching descriptor vectors using FLANN matcher
    FlannBasedMatcher matcher;
    vector<DMatch> matches;
    matcher.match(img1.descriptors, img2.descriptors, matches);
    vector<DMatch> matches_back;
    matcher.match(img2.descriptors, img1.descriptors, matches_back);


    map<int,int> bm;
    for (auto &m : matches_back) {
        bm[m.queryIdx] = m.trainIdx;
    }


    //cout << "m: " << matches.size() << endl;
    //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
    vector<DMatch> good_matches;

    for (auto &matche : matches) {
        auto s1 = img1.keypoints[matche.queryIdx].size;
        auto s2 = img2.keypoints[matche.trainIdx].size;
        float m = max(s2, s1) / min(s2, s1);
        if (matche.distance < 0.3 && m < 1.5 && bm[matche.trainIdx] == matche.queryIdx) {
            good_matches.push_back(matche);
        }
    }

    //-- Quick calculation of max and min distances between keypoints
    /*


    double max_dist = 0;
    double min_dist = 100;
     for (auto &matche : good_matches) {
        double dist = matche.distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }*/

    //printf("-- Max dist : %f \n", max_dist);
    //printf("-- Min dist : %f \n", min_dist);
    cout << "gm: " << good_matches.size() << endl;

    Mat img_matches;
    drawMatches(img1.mat, img1.keypoints, img2.mat, img2.keypoints,
                good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    return img_matches;
}

void show(const Mat &image) {
    auto *window = new MainWindow;

    window->setImage(image);

    window->show();
}