//
// Created by agabor on 2017.09.13..
//

#include <functional>

#include <QtWidgets/QLabel>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>

#include "ConfigWidget.h"

ConfigWidget::ConfigWidget(QWidget *parent) : QDockWidget("config", parent) {
    auto *widget = new QWidget(this);
    auto *layout = new QHBoxLayout;

    addDoubleParameter(layout, "contrast", settings.contrast, [=](double val) {
        settings.contrast = val;
    });

    addDoubleParameter(layout, "minHessian", settings.minHessian, [=](double val) {
        settings.minHessian = val;
    });

    addDoubleParameter(layout, "octave 0 scale", settings.scale0, [=](double val) {
        settings.scale0 = static_cast<float>(val);
    });

    addDoubleParameter(layout, "octave 1 scale", settings.scale1, [=](double val) {
        settings.scale1 = static_cast<float>(val);
    });

    addDoubleParameter(layout, "octave 2 scale", settings.scale2, [=](double val) {
        settings.scale2 = static_cast<float>(val);
    });

    addDoubleParameter(layout, "octave 3 scale", settings.scale3, [=](double val) {
        settings.scale3 = static_cast<float>(val);
    });


    auto *propertyLayout = new QVBoxLayout;
    propertyLayout->addWidget(new QCheckBox("reverse matching"));
    propertyLayout->addWidget(new QCheckBox("scaled matching"));
    layout->addItem(propertyLayout);
    widget->setLayout(layout);

    setWidget(widget);
}

void ConfigWidget::addDoubleParameter(QLayout *layout, QString name, double value, std::function<void (double)> setter) {
    auto *propertyLayout = new QVBoxLayout;
    propertyLayout->addWidget(new QLabel(name));
    auto *spinBox = new QDoubleSpinBox();
    spinBox->setMaximum(1000);
    spinBox->setValue(value);
    connect(spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double val) {
        changed = true;
        setter(val);
    });
    propertyLayout->addWidget(spinBox);
    layout->addItem(propertyLayout);
}
