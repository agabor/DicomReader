
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


    QList<QSharedPointer<Image>> images;
    for (auto& file :  dir.entryList()) {
        if (file.startsWith('.'))
            continue;
        const QString absoluteFilePath = dir.absoluteFilePath(file);
        if (DicomReader::isDicomFile(absoluteFilePath.toLatin1().data())) {
            auto *i = new Image(absoluteFilePath.toLatin1().data());
            i->file_name = file.toLatin1().data();
            images.push_back(QSharedPointer<Image>(i));
        }
    }

    auto *window = new MainWindow;


    window->show();
    window->init(images);

    return app.exec();
}



