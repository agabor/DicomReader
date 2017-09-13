//
// Created by agabor on 2017.09.13..
//

#ifndef CV_TEST_IMAGE_H
#define CV_TEST_IMAGE_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/nonfree/nonfree.hpp"
#include <opencv2/calib3d/calib3d.hpp>
#include "DicomReader.h"

using namespace cv;
using namespace std;

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
        //   cout <<  double(templ.mat.rows * templ.mat.cols) / double(mat.rows * mat.cols) << endl;
    }

    void scan(cv::SurfFeatureDetector &detector){
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


#endif //CV_TEST_IMAGE_H
