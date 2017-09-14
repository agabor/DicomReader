//
// Created by agabor on 2017.09.14..
//

#include <QtWidgets/QProgressDialog>
#include <QtCore/QCoreApplication>

#include "ProgressDialog.h"

QProgressDialog *dialog;
int idx;

void ProgressDialog::start(QString title, int items) {
    dialog = new QProgressDialog;
    dialog->setWindowTitle(title);
    dialog->setLabelText(title);
    dialog->setCancelButton(nullptr);
    dialog->setMaximum(items);
    dialog->show();
    qApp->processEvents();
    idx = 0;
}

void ProgressDialog::step() {
    dialog->setValue(++idx);
    qApp->processEvents();
}

void ProgressDialog::end() {
    dialog->setValue(dialog->maximum());
    dialog->close();
    qApp->processEvents();
    delete dialog;
    dialog = nullptr;
}
