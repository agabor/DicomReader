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

using namespace cv;
using namespace Qt;

void MainWindow::setImage(Mat &image) {
    imageLabel->setFixedSize(image.cols, image.rows);

    imageLabel->setPixmap(
            QPixmap::fromImage(QImage(image.data, image.cols, image.rows, int(image.step), image.type() == CV_8U ? QImage::Format_Grayscale8 : QImage::Format_RGB888)));
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
    button_dock->setWidget(new QPushButton(QObject::tr("Filter"), button_dock));
    addDockWidget(Qt::BottomDockWidgetArea, button_dock);
    for (auto& file : files) {
        images.push_back(new Image(file.toLatin1().data()));
    }
    setImage(images[0]->mat);

    connect(filesView, &QListView::activated,[=]( const QModelIndex &index ) { setImage(images[index.row()]->mat);} );
}

