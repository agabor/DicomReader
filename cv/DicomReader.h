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
    static bool isDicomFile(const char* file_name);
    void addFile(const char* file_name);
    void config();
    std::vector<cv::Mat> read(int cv_type = -1);
private:
    int depth = 0;
    int input_bits = 0;
    int valuable_bits = 0;
    int width = 0;
    int height = 0;
    size_t size = 0;
    std::vector<DicomImage*> images;
    u_int32_t min = (u_int32_t)-1;
    u_int32_t max = 0;

    template <typename T>
    void configureNormalization(DicomImage *img);
    void configureNormalization(DicomImage *img);

    template<typename T>
    cv::Mat createMat(DicomImage *img, int cv_type) const;

    template <typename IT, typename OT>
    OT* normalize(const IT* data) const;

    template <typename IT, typename OT>
    OT* normalizeToFloat(const IT* data) const;

    int guessCVType() const;


    void calculateValuableBits();

    void readImageProperties(const DicomImage *img);

    template<typename T>
    const T *getOutputData(DicomImage *img) const;
};


#endif //CV_TEST_DICOM_H
