//
// Created by agabor on 2017.09.11..
//

#include "MainWindow.h"

#include <qt5/QtWidgets/QLabel>
#include <qt5/QtWidgets/QLayout>
#include <qt5/QtWidgets/QVBoxLayout>
#include <qt5/QtGui/QImage>

using namespace cv;

void MainWindow::setImage(Mat image) {
    setMinimumSize(image.cols, image.rows);
    QLabel *label = new QLabel;
    label->setFixedSize(image.cols, image.rows);
    //window->lay
    //window->setLayout(new QVBoxLayout);
    setCentralWidget(label);

    label->setPixmap(
            QPixmap::fromImage(QImage(image.data, image.cols, image.rows, int(image.step), QImage::Format_RGB888)));
}

