//
// Created by agabor on 2017.09.13..
//

#ifndef CV_TEST_CONFIGWIDGET_H
#define CV_TEST_CONFIGWIDGET_H


#include <QtWidgets/QWidget>
#include <QtWidgets/QDockWidget>

#include "Image.h"

class ConfigWidget : public QDockWidget {
public:
    explicit ConfigWidget(QWidget* parent);
    MatchSettings settings;
};


#endif //CV_TEST_CONFIGWIDGET_H
