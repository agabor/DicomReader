#include "Image.h"
#include "DicomReader.h"
#include <iostream>

using namespace cv;
using namespace std;



Image::Image(const char *file_name) {
    DicomReader r;
    original = r.read(file_name, CV_8U);
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
    if (settings.mirrorY) {
        mirrored = std::make_shared<Image>();
        mirrored->file_name = file_name;
        flip(original, mirrored->original, 1);
        mirrored->scanSelf(settings);
    }

    scanSelf(settings);
}

void Image::scanSelf(const MatchSettings &settings) {
    applyContrast(settings);

    clear();

    detectKeyPoints(settings);

    extractDescriptors();

}

void Image::clear() {
    keypoints.clear();
    descriptors.clear();
    scaled_keypoints.clear();
    scaled_descriptors.clear();
}


KeyPoint getScaledKeyPoint(const MatchSettings &settings, const KeyPoint &k) {
    KeyPoint scaled;
    scaled.size = k.size * getScale(k.octave, settings);
    scaled.octave = k.octave;
    scaled.response = k.response;
    scaled.angle = k.angle;
    scaled.class_id = k.class_id;
    scaled.pt = k.pt;
    return scaled;
}


void Image::detectKeyPoints(const MatchSettings &settings)  {
    SurfFeatureDetector detector{settings.minHessian, OCTAVES, 1};
    vector<KeyPoint> points;
    detector.detect(mat, points );

    sort(points.begin(), points.end(),
         [](const KeyPoint &a, const KeyPoint &b) -> bool
         {
             return a.response > b.response;
         });

    for(auto &k : points) {
        addKeyPoint(k, keypoints);
        KeyPoint scaled = getScaledKeyPoint(settings, k);
        addKeyPoint(scaled, scaled_keypoints);
    }
}

void Image::extractDescriptors() {
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





