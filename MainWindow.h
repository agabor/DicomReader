//
// Created by agabor on 2017.09.11..
//

#ifndef CV_TEST_MAINWINDOW_H
#define CV_TEST_MAINWINDOW_H


#include <opencv2/core/core.hpp>
#include <qt5/QtWidgets/QMainWindow>

class MainWindow : public QMainWindow {
public:
    void setImage(cv::Mat image);
};


#endif //CV_TEST_MAINWINDOW_H
