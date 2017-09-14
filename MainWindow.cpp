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

    QDockWidget *left_dock = new QDockWidget(QObject::tr("Files"), this);
    left_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    filesView = new QListView(left_dock);
    left_dock->setWidget(filesView);
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


    auto *configWidget = new ConfigWidget(this);
    configWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::TopDockWidgetArea, configWidget);

    QStringList dicomFiles;
    for (auto& file : files) {
        const QString absoluteFilePath = base_dir.absoluteFilePath(file);
        if (DicomReader::isDicomFile(absoluteFilePath.toLatin1().data())) {

            dicomFiles << file;
            auto *i = new Image(absoluteFilePath.toLatin1().data());

            i->scan(configWidget->settings);
            images.push_back(i);
        }
    }

    auto m = new QStringListModel();
    m->setStringList(dicomFiles);
    filesView->setModel(m);
    currentImage = images[0];
    setImage(currentImage->original);

    connect(filesView, &QListView::activated,[=]( const QModelIndex &index ) {
        currentImage = images[index.row()];
        setImage(currentImage->original);
    });

    connect(button, &QPushButton::pressed,[=]( ) {
        matches.clear();
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
    });


    connect(filteredView, &QListView::activated,[=]( const QModelIndex &index ) {
        setImage(matches[index.row()]);
    });
}

