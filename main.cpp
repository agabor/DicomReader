
#include <opencv2/core/core.hpp>
#include "DicomReader.h"
#include <iostream>
#include <qt5/QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include "MainWindow.h"


using namespace cv;
using namespace std;


int main(int argc, char** argv)
{

    std::cout << "open cv " << CV_MAJOR_VERSION << '.' << CV_MINOR_VERSION << std::endl;

    QApplication app(argc, argv);

    QString dirPath = QFileDialog::getExistingDirectory(nullptr, QObject::tr("Open Directory"),
                                                    "/home/agabor/CLionProjects/cv_test/input",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    QDir dir(dirPath);

    auto *window = new MainWindow;


    window->show();
    window->init(dir, dir.entryList());

    return app.exec();
}



