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


    addCheckBoxes(layout);

    widget->setLayout(layout);

    setWidget(widget);
}

void ConfigWidget::addCheckBoxes(QLayout *layout) {
    auto *propertyLayout = new QVBoxLayout;
    addBooleanParameter(propertyLayout, "reverse matching", settings.reverse, [=](bool checked) {
        settings.reverse = checked;
    });
    addBooleanParameter(propertyLayout, "scaled matching", settings.scale, [=](bool checked) {
        settings.scale = checked;
    });
    addBooleanParameter(propertyLayout, "mirror Y", settings.mirrorY, [=](bool checked) {
        settings.mirrorY = checked;
    });

    layout->addItem(propertyLayout);
}

void
ConfigWidget::addBooleanParameter(QLayout *layout, QString name, bool value, std::function<void (bool)> setter) {
    auto *reverseCB = new QCheckBox(name);
    reverseCB->setChecked(value);
    connect(reverseCB, &QCheckBox::toggled, [=](bool val) {
        changed = true;
        setter(val);
    });
    layout->addWidget(reverseCB);
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
