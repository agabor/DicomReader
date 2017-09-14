
#include <opencv2/core/core.hpp>
#include "cv/DicomReader.h"
#include <iostream>
#include <qt5/QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QProgressDialog>
#include "ui/MainWindow.h"


using namespace cv;
using namespace std;


int main(int argc, char** argv)
{

    std::cout << "OpenCV " << CV_MAJOR_VERSION << '.' << CV_MINOR_VERSION << std::endl;

    QApplication app(argc, argv);

    QString dirPath = QFileDialog::getExistingDirectory(nullptr, QObject::tr("Open Directory"),
                                                    "/home/agabor/CLionProjects/cv_test/input",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    QDir dir(dirPath);


    QStringList files = dir.entryList();
    auto *dialog = new QProgressDialog;
    dialog->setLabelText("Loading Files");
    dialog->setCancelButton(nullptr);
    dialog->setMaximum(files.size());
    int idx = 0;
    dialog->show();
    std::vector<std::shared_ptr<Image>> images;
    for (auto& file :  files) {
        dialog->setValue(idx++);
        app.processEvents();
        if (file.startsWith('.'))
            continue;
        const QString absoluteFilePath = dir.absoluteFilePath(file);
        if (DicomReader::isDicomFile(absoluteFilePath.toLatin1().data())) {
            auto *i = new Image(absoluteFilePath.toLatin1().data());
            i->file_name = file.toLatin1().data();
            images.push_back(std::shared_ptr<Image>(i));
        }
    }
    dialog->setValue(files.size());
    app.processEvents();
    dialog->close();

    auto *window = new MainWindow(images);


    window->show();

    return app.exec();
}



