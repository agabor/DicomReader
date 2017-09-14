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
#include "../cv/ImagePair.h"

#include "ConfigWidget.h"

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(std::vector<std::shared_ptr<Image>> images);
private slots:
    void runSURF();
private:
    void setImage(cv::Mat &image);

    void initFilesWidget();

    void initMatchesWidget();

    void initFilterButton();

    void runFeatureDetectionAndDescription();

    void runFeatureMatching();

    ImagePair getImagePair(const std::shared_ptr<Image> &img) const;

    void setMatchLabels() const;

    void sortMatches();

    void setFiles(const std::vector<std::shared_ptr<Image>> &images);

    void initConfigWidget();

    QLabel *imageLabel;
    QListView *filesView;
    QListView *filteredView;
    ConfigWidget *configWidget;

    std::vector<ImagePair> matches;
    std::vector<std::shared_ptr<Image>> images;
    std::shared_ptr<Image> currentImage;
};


#endif //CV_TEST_MAINWINDOW_H
