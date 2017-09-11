//
// Created by agabor on 2017.09.08..
//

#include "DicomReader.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include <opencv2/core/core.hpp>

void DicomReader::addFile(const char *file_name) {
    images.push_back(new DicomImage(file_name));
}

template <typename T>
std::tuple<T,T, size_t> getRange(const T* data) {
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

template <typename IT, typename OT>
OT* DicomReader::normalize(const IT* data) const {
    auto *normd = new OT[size];
    auto maxval = (OT)-1;
    IT d = (IT)max - (IT)min;
    auto m =(double)maxval/(double)d;
    for( size_t i = 0; i < size; ++i){
        normd[i] = (data[i] - min) * m;
    }
    return normd;
}


template <typename IT, typename OT>
OT* DicomReader::normalizeToFloat(const IT* data) const {
    auto *normd = new OT[size];
    auto d = (double)((IT)max - (IT)min);
    for( size_t i = 0; i < size; ++i){
        normd[i] = (OT)((data[i] - min) / d);
    }
    return normd;
}

void DicomReader::config() {
    for (auto img : images) {
        readImageProperties(img);
        configureNormalization(img);
    }
    calculateValuableBits();
}

void DicomReader::readImageProperties(const DicomImage *img) {
    auto w = int(img->getWidth());
    auto h = int(img->getHeight());
    if (input_bits == 0) {
        depth = img->getDepth();
        width = w;
        height = h;
        input_bits = 1;
        while (input_bits < depth)
            input_bits *= 2;
    } else {
        if (depth != img->getDepth())
            throw std::exception();
        if (width != w)
            throw std::exception();
        if (height != h)
            throw std::exception();
    }
}

void DicomReader::calculateValuableBits() {
    auto interval = max - min;
    while (interval != 0) {
        interval /= 2;
        ++valuable_bits;
    }
}

void DicomReader::configureNormalization(DicomImage *img) {
    switch (input_bits) {
            case 8:
                configureNormalization<uint8_t>(img);
                break;
            case 16:
                configureNormalization<uint16_t>(img);
                break;
            case 32:
                configureNormalization<uint32_t>(img);
                break;
            case 64:
                configureNormalization<uint64_t>(img);
                break;
            default:
                throw std::exception();
        }
}

template<typename T>
void DicomReader::configureNormalization(DicomImage *img) {
    T min, max;
    size_t size;
    auto *data = getOutputData<T>(img);
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

std::vector<cv::Mat> DicomReader::read(int cv_type) {
    auto result = std::vector<cv::Mat>();

    if (cv_type == -1)
        cv_type = guessCVType();

    for (auto &img : images) {
        cv::Mat m;
        switch (input_bits) {
            case 8:
                m = createMat<uint8_t>(img, cv_type);
                break;
            case 16:
                m = createMat<uint16_t>(img, cv_type);
                break;
            case 32:
                m = createMat<uint32_t>(img, cv_type);
                break;
            case 64:
                m = createMat<uint64_t>(img, cv_type);
                break;
            default:
                throw std::exception();
        }
        result.push_back(m);
    }
    return result;

}

int DicomReader::guessCVType() const {
    if (valuable_bits <= 8) {
        return CV_8U;
    } else if (valuable_bits <= 16) {
        return CV_16U;
    } else if (valuable_bits <= 32) {
        return CV_32F;
    } if (valuable_bits <= 64) {
        return CV_64F;
    }
    return CV_8U;
}

template<typename T>
cv::Mat DicomReader::createMat(DicomImage *img, int cv_type) const {
    void *data;

    auto *img_data = getOutputData<T>(img);
    switch (cv_type) {
        case CV_8U:
            data = normalize<T, uint8_t>(img_data);
            break;
        case CV_16U:
            data = normalize<T, uint16_t>(img_data);
            break;
        case CV_32F:
            data = normalizeToFloat<T, float>(img_data);
            break;
        case CV_64F:
            data = normalizeToFloat<T, double>(img_data);
            break;
        default:
            throw std::exception();
    }

    return cv::Mat(height, width, cv_type, data);
}

template<typename T>
const T *DicomReader::getOutputData(DicomImage *img) const
{
    return static_cast<const T*>(img->getOutputData(sizeof(T) * 8));
}


