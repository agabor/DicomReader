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

using namespace cv;
using namespace std;
using namespace Qt;

void MainWindow::setImage(Mat &image) {
    imageLabel->setFixedSize(image.cols, image.rows);
    auto format = image.type() == CV_8U ? QImage::Format_Grayscale8 : QImage::Format_RGB888;
    const QImage &qImage = QImage(image.data, image.cols, image.rows, int(image.step), format);
    imageLabel->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::init(QStringList files) {
    imageLabel = new QLabel;
    setCentralWidget(imageLabel);

    QDockWidget *left_dock = new QDockWidget(QObject::tr("Files"), this);
    left_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    filesView = new QListView(left_dock);
    left_dock->setWidget(filesView);
    auto m = new QStringListModel();
    m->setStringList(files);
    filesView->setModel(m);
    addDockWidget(Qt::LeftDockWidgetArea, left_dock);

    QDockWidget *right_dock = new QDockWidget(QObject::tr("Files"), this);
    right_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    filteredView = new QListView(right_dock);
    right_dock->setWidget(filteredView);
    addDockWidget(Qt::RightDockWidgetArea, right_dock);


    QDockWidget *button_dock = new QDockWidget(this);
    button_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    QPushButton *button = new QPushButton(QObject::tr("Filter"), button_dock);
    button_dock->setWidget(button);
    addDockWidget(Qt::BottomDockWidgetArea, button_dock);
    cv::SurfFeatureDetector detector{200,OCTAVES,1};
    for (auto& file : files) {
        Image *i = new Image(file.toLatin1().data());
        i->scan(detector);
        images.push_back(i);
    }
    currentImage = images[0];
    setImage(currentImage->mat);

    connect(filesView, &QListView::activated,[=]( const QModelIndex &index ) {
        currentImage = images[index.row()];
        setImage(currentImage->mat);
    });

    connect(button, &QPushButton::pressed,[=]( ) {
        matches.clear();
        QStringList names;
        for (auto img : images) {
            if (img == currentImage)
                continue;
            int count;
            Mat mimg;
            tie(count, mimg) = currentImage->match(*img);
            if (count > 0) {
                names << QString::number(count);
                matches.push_back(mimg);
            }
        }

        auto model = new QStringListModel();
        model->setStringList(names);
        filteredView->setModel(model);
    });


    connect(filteredView, &QListView::activated,[=]( const QModelIndex &index ) {
        setImage(matches[index.row()]);
    });
}

