//
// Created by agabor on 2017.09.13..
//

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDoubleSpinBox>
#include "ConfigWidget.h"
#include "Image.h"

ConfigWidget::ConfigWidget(QWidget *parent) : QDockWidget("config", parent) {
    auto *widget = new QWidget(this);
    auto *layout = new QHBoxLayout;

    auto *contrastLayout = new QVBoxLayout;
    contrastLayout->addWidget(new QLabel(QObject::tr("contrast")));
    auto *contrastSB = new QDoubleSpinBox();
    contrastSB->setValue(settings.contrast);
    connect(contrastSB, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [ = ](double val){
        settings.contrast = val;
    });
    contrastLayout->addWidget(contrastSB);
    layout->addItem(contrastLayout);

    auto *minHessianLayout = new QVBoxLayout;
    minHessianLayout->addWidget(new QLabel(QObject::tr("minHessian")));
    auto *hessianSB = new QDoubleSpinBox();
    hessianSB->setMaximum(1000);
    hessianSB->setValue(settings.minHessian);
    connect(hessianSB, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [ = ](double val){
        settings.minHessian = val;
    });
    minHessianLayout->addWidget(hessianSB);
    layout->addItem(minHessianLayout);

    auto *oct0Layout = new QVBoxLayout;
    oct0Layout->addWidget(new QLabel(QObject::tr("octave 0 scale")));
    auto *oct0SB = new QDoubleSpinBox();
    oct0SB->setValue(settings.scale0);
    connect(oct0SB, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [ = ](double val){
        settings.scale0 = static_cast<float>(val);
    });
    oct0Layout->addWidget(oct0SB);
    layout->addItem(oct0Layout);

    auto *oct1Layout = new QVBoxLayout;
    oct1Layout->addWidget(new QLabel(QObject::tr("octave 1 scale")));
    auto *oct1SB = new QDoubleSpinBox();
    oct1SB->setValue(settings.scale1);
    connect(oct1SB, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [ = ](double val){
        settings.scale0 = static_cast<float>(val);
    });
    oct1Layout->addWidget(oct1SB);
    layout->addItem(oct1Layout);

    auto *oct2Layout = new QVBoxLayout;
    oct2Layout->addWidget(new QLabel(QObject::tr("octave 2 scale")));
    auto *oct2SB = new QDoubleSpinBox();
    oct2SB->setValue(settings.scale2);
    connect(oct2SB, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [ = ](double val){
        settings.scale0 = static_cast<float>(val);
    });
    oct2Layout->addWidget(oct2SB);
    layout->addItem(oct2Layout);

    auto *oct3Layout = new QVBoxLayout;
    oct3Layout->addWidget(new QLabel(QObject::tr("octave 3 scale")));
    auto *oct3SB = new QDoubleSpinBox();
    oct3SB->setValue(settings.scale3);
    connect(oct3SB, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [ = ](double val){
        settings.scale3 = static_cast<float>(val);
    });
    oct3Layout->addWidget(oct3SB);
    layout->addItem(oct3Layout);

    widget->setLayout(layout);

    setWidget(widget);
}
