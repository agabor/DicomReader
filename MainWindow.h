//
// Created by agabor on 2017.09.11..
//

#ifndef CV_TEST_MAINWINDOW_H
#define CV_TEST_MAINWINDOW_H


#include <opencv2/core/core.hpp>
#include <qt5/QtWidgets/QMainWindow>
#include <QLabel>
#include <QtWidgets/QListView>
#include "Image.h"

class MainWindow : public QMainWindow {
public:
    void init(QStringList files);
    void setImage(cv::Mat &image);
private:
    QLabel *imageLabel;
    QListView *filesView;
    QListView *filteredView;
    QVector<Image*> matches;
    QVector<Image*> images;
};


#endif //CV_TEST_MAINWINDOW_H
