//
// Created by agabor on 2017.09.08..
//

#include "dicom.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include <opencv2/core/core.hpp>
#include <memory>
#include <tuple>

void DicomReader::addFile(const char *file_name) {
    images.emplace_back(new DicomImage(file_name));
}

template <typename T>
std::tuple<T,T, size_t> getRange(T* data) {
    size_t i = 0;
    T item = data[i];
    T min = item;
    T max = item;
    while(item != 0) {
        if (item < min) {
            min = item;
        }
        if (item > max) {
            max = item;
        }
        item = data[++i];
    }
    return std::tuple<T,T, size_t>(min, max, i);
}

template <typename T>
uint16_t* normalize(T* data, size_t size, T min, T max) {
    auto *normd = new uint16_t[size];
    auto maxval = (uint16_t)-1;
    T d = max - min;
    auto m =(float)((double)maxval/(double)d);
    for( size_t i = 0; i < size; ++i){
        normd[i] = (data[i] - min) * m;
    }
    return normd;
}

void DicomReader::config() {
    for (auto img : images) {
        if (bits == 0) {
            depth = img->getDepth();
            width = static_cast<int>(img->getWidth());
            height = static_cast<int>(img->getHeight());

            if (depth <= 8) {
                type = CV_8U;
                bits = 8;
            } else if (depth <= 16) {
                type = CV_16U;
                bits = 16;
            } else if (depth <= 32) {
                type = CV_32S;
                bits = 32;
            }
        } else
            if (depth != img->getDepth())
                throw std::exception();
        switch (bits) {
            case 8:
                configureNormalization<uint8_t>(img);
                break;
            case 16:
                configureNormalization<uint16_t>(img);
                break;
            case 32:
                configureNormalization<uint32_t>(img);
                break;
            default:break;
        }
    }
}

template<typename T>
void DicomReader::configureNormalization(DicomImage *img) {
    T min, max;
    size_t size;
    auto *data = (T *) img->getOutputData(sizeof(T) * 8);
    std::tie(min, max, size) = getRange<T>(data);
    if (min < this->min)
        this->min = min;
    if (max > this->max)
        this->max = max;
    if (this->size == 0)
        this->size = size;
    else if (this->size != size)
        throw std::exception();
}

std::vector<cv::Mat> DicomReader::read() {
    auto result = std::vector<cv::Mat>();
    for (auto &img : images) {
        switch (bits) {
            case 8:
                result.push_back(createMat<uint8_t>(img));
                break;
            case 16:
                result.push_back(createMat<uint16_t>(img));
                break;
            case 32:
                result.push_back(createMat<uint32_t>(img));
                break;
            default:break;
        }
    }
    return result;

}

template<typename T>
cv::Mat DicomReader::createMat(DicomImage *img) const {
    uint16_t *data = normalize((T *)img->getOutputData(sizeof(T)*8), size, (T) min, (T) max);
    return cv::Mat(height, width, CV_16U, data);
}

