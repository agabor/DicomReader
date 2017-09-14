#include <iostream>

#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>

#include <opencv2/core/core.hpp>

#include "cv/DicomReader.h"

#include "ui/MainWindow.h"
#include "ui/ProgressDialog.h"


using namespace cv;
using namespace std;

vector<shared_ptr<Image>> readImages(const QApplication &app, const QString &dirPath);

int main(int argc, char** argv)
{

    std::cout << "OpenCV " << CV_MAJOR_VERSION << '.' << CV_MINOR_VERSION << std::endl;

    QApplication app(argc, argv);

    QString dirPath = QFileDialog::getExistingDirectory(nullptr, QObject::tr("Open Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (dirPath.isEmpty())
        return 0;

    vector<shared_ptr<Image>> images = readImages(app, dirPath);

    auto *window = new MainWindow(images);


    window->show();

    return app.exec();
}

vector<shared_ptr<Image>> readImages(const QApplication &app, const QString &dirPath) {
    QDir dir(dirPath);
    QStringList files = dir.entryList();
    ProgressDialog::start("Loading Files", files.size());
    vector<shared_ptr<Image>> images;
    for (auto &file :  files) {
        ProgressDialog::step();
        if (file.startsWith('.'))
            continue;
        const QString absoluteFilePath = dir.absoluteFilePath(file);
        if (DicomReader::isDicomFile(absoluteFilePath.toLatin1().data())) {
            auto *i = new Image(absoluteFilePath.toLatin1().data());
            i->file_name = file.toLatin1().data();
            images.push_back(shared_ptr<Image>(i));
        }
    }
    ProgressDialog::end();
    return images;
}



