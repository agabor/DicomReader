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
#include "DicomReader.h"
#include "ConfigWidget.h"

using namespace cv;
using namespace std;
using namespace Qt;

void MainWindow::setImage(Mat &image) {
    imageLabel->setFixedSize(image.cols, image.rows);
    auto format = image.type() == CV_8U ? QImage::Format_Grayscale8 : QImage::Format_RGB888;
    const QImage &qImage = QImage(image.data, image.cols, image.rows, int(image.step), format);
    imageLabel->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::init(QDir base_dir, QStringList files) {
    imageLabel = new QLabel;
    setCentralWidget(imageLabel);

    initFilesWidget();

    initCategoryWidget();

    initFilterButton();

    configWidget = new ConfigWidget(this);
    configWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::TopDockWidgetArea, configWidget);

    QStringList dicomFiles;
    for (auto& file : files) {
        const QString absoluteFilePath = base_dir.absoluteFilePath(file);
        if (DicomReader::isDicomFile(absoluteFilePath.toLatin1().data())) {

            dicomFiles << file;
            auto *i = new Image(absoluteFilePath.toLatin1().data());

            images.push_back(i);
        }
    }

    auto m = new QStringListModel();
    m->setStringList(dicomFiles);
    filesView->setModel(m);
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
    connect(filteredView, &QListView::activated,[=]( const QModelIndex &index ) {
        setImage(matches[index.row()]);
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
            configWidget->changed = false;
            for (auto *img : images) {
                img->scan(configWidget->settings);
            }
    }
    QStringList names;
    for (auto img : images) {
            if (img == currentImage)
                continue;
            int count;
            Mat mimg;
            tie(count, mimg) = currentImage->match(*img, configWidget->settings);
            if (count > 0) {
                names << QString::number(count);
                matches.push_back(mimg);
            }
        }

    auto model = new QStringListModel();
    model->setStringList(names);
    filteredView->setModel(model);
}

