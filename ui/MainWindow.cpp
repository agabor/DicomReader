//
// Created by agabor on 2017.09.11..
//

#include "MainWindow.h"

#include <qt5/QtWidgets/QLabel>
#include <qt5/QtWidgets/QLayout>
#include <qt5/QtWidgets/QVBoxLayout>
#include <qt5/QtGui/QImage>
#include <QtCore/QStringListModel>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QPushButton>
#include <tuple>
#include <QtWidgets/QProgressDialog>
#include <QtCore/QCoreApplication>
#include "../cv/DicomReader.h"
#include "ConfigWidget.h"
#include "../cv/ImagePair.h"

using namespace cv;
using namespace std;
using namespace Qt;

void MainWindow::setImage(Mat &image) {
    imageLabel->setFixedSize(image.cols, image.rows);
    auto format = image.type() == CV_8U ? QImage::Format_Grayscale8 : QImage::Format_RGB888;
    const QImage &qImage = QImage(image.data, image.cols, image.rows, int(image.step), format);
    imageLabel->setPixmap(QPixmap::fromImage(qImage));
    adjustSize();
}

MainWindow::MainWindow(std::vector<std::shared_ptr<Image>> images) {
    this->images = images;
    imageLabel = new QLabel;
    setCentralWidget(imageLabel);

    initFilesWidget();

    initCategoryWidget();

    initFilterButton();

    configWidget = new ConfigWidget(this);
    configWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::TopDockWidgetArea, configWidget);


    QStringList fileNames;
    for (const auto &img : images) {
        fileNames << img->file_name.c_str();
    }
    auto m = new QStringListModel();
    m->setStringList(fileNames);
    filesView->setModel(m);
    filesView->setFixedWidth(200);
    currentImage = images[0];
    setImage(currentImage->original);
}

void MainWindow::initFilterButton() {
    QDockWidget *button_dock = new QDockWidget(this);
    button_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    QPushButton *button = new QPushButton(QObject::tr("Filter"), button_dock);
    button_dock->setWidget(button);
    addDockWidget(BottomDockWidgetArea, button_dock);
    connect(button, &QPushButton::pressed, this, &MainWindow::runSURF);
}

void MainWindow::initCategoryWidget() {
    QDockWidget *right_dock = new QDockWidget(QObject::tr("Files"), this);
    right_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    filteredView = new QListView(right_dock);
    right_dock->setWidget(filteredView);
    addDockWidget(RightDockWidgetArea, right_dock);
    filesView->setFixedWidth(200);
    connect(filteredView, &QListView::activated,[=]( const QModelIndex &index ) {
        setImage(matches[index.row()].image_b->original);
    });
    connect(filteredView, &QListView::doubleClicked,[=]( const QModelIndex &index ) {
        cv::imshow("Matched Features", matches[index.row()].matchImage());
    });
}

void MainWindow::initFilesWidget() {
    QDockWidget *left_dock = new QDockWidget(QObject::tr("Files"), this);
    left_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    filesView = new QListView(left_dock);
    left_dock->setWidget(filesView);
    addDockWidget(LeftDockWidgetArea, left_dock);
    connect(filesView, &QListView::activated,[=]( const QModelIndex &index ) {
        currentImage = images[index.row()];
        setImage(currentImage->original);
    });
}

void MainWindow::runSURF() {
    matches.clear();
    if (configWidget->changed) {
        runFeatureDetectionAndDescription();
    }
    runFeatureMatching();

}

void MainWindow::runFeatureMatching() {
    QStringList names;
    auto *dialog = new QProgressDialog(this);
    dialog->setLabelText("Feature Matching");
    dialog->setCancelButton(nullptr);
    dialog->setMaximum(images.size() - 1);
    int idx = 0;
    dialog->show();
    qApp->processEvents();
    for (const auto &img : images) {
        if (img == currentImage)
            continue;

        ImagePair imagePair = getImagePair(img);

        if (imagePair.matchCount() > 0) {
            names << imagePair.label().c_str();
            matches.push_back(imagePair);
        }
        dialog->setValue(++idx);
        qApp->processEvents();
    }

    auto model = new QStringListModel();
    model->setStringList(names);
    filteredView->setModel(model);
    dialog->close();
}

ImagePair MainWindow::getImagePair(const shared_ptr<Image> &img) const {
    ImagePair imagePair(configWidget->settings, currentImage, img);

    if (configWidget->settings.mirrorY) {
        ImagePair mirroredPair(configWidget->settings, currentImage, img->mirrored);
        if (mirroredPair.matchCount() > imagePair.matchCount())
            return mirroredPair;
    }
    return imagePair;
}

void MainWindow::runFeatureDetectionAndDescription() {
    configWidget->changed = false;
    auto *dialog = new QProgressDialog(this);
    dialog->setLabelText("Feature Detection & Description");
    dialog->setCancelButton(nullptr);
    dialog->setMaximum(images.size());
    int idx = 0;
    dialog->show();
    qApp->processEvents();

    for (const auto &img : images) {
        img->scan(configWidget->settings);
        dialog->setValue(++idx);
        qApp->processEvents();
    }
    dialog->close();
}
