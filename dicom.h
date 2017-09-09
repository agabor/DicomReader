//
// Created by agabor on 2017.09.08..
//

#ifndef CV_TEST_DICOM_H
#define CV_TEST_DICOM_H

#include <vector>
#include <memory>

namespace cv {
    class Mat;
}

class DicomImage;

class DicomReader {
public:
    void addFile(const char* file_name);
    void config();
    std::vector<cv::Mat> read();
private:
    int bits = 0;
    int depth = 0;
    int type = 0;
    int width = 0;
    int height = 0;
    size_t size = 0;
    std::vector<DicomImage*> images;
    u_int32_t min = (u_int32_t)-1;
    u_int32_t max = 0;

    template <typename T>
    void configureNormalization(DicomImage *img);

    template<typename T>
    cv::Mat createMat(DicomImage *img) const;
};


#endif //CV_TEST_DICOM_H
