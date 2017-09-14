//
// Created by agabor on 2017.09.11..
//

#ifndef CV_TEST_MAINWINDOW_H
#define CV_TEST_MAINWINDOW_H

#include <memory>

#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QtWidgets/QListView>
#include <QtCore/QDir>

#include <opencv2/core/core.hpp>

#include "../cv/Image.h"
#include "ConfigWidget.h"
#include "../cv/ImagePair.h"

class MainWindow : public QMainWindow {
public:
    MainWindow(std::vector<std::shared_ptr<Image>> images);
    void setImage(cv::Mat &image);
private slots:
    void runSURF();
private:
    QLabel *imageLabel;
    QListView *filesView;
    QListView *filteredView;
    ConfigWidget *configWidget;

    std::vector<ImagePair> matches;
    std::vector<std::shared_ptr<Image>> images;
    std::shared_ptr<Image> currentImage;

    void initFilesWidget();

    void initCategoryWidget();

    void initFilterButton();

    void runFeatureDetectionAndDescription();

    void runFeatureMatching();

    ImagePair getImagePair(const std::shared_ptr<Image> &img) const;
};


#endif //CV_TEST_MAINWINDOW_H
