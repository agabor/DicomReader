//
// Created by agabor on 2017.09.14..
//

#ifndef CV_TEST_PROGRESS_H
#define CV_TEST_PROGRESS_H


#include <QtCore/QString>

class ProgressDialog {
public:
    static void start(QString title, int items);
    static void step();
    static void end();
};


#endif //CV_TEST_PROGRESS_H
