/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>
#include "view.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    View *view;
    QDockWidget *controls;
    QWidget *control_objects;
    QCheckBox *shadows;
    QCheckBox *ambient_occlusion;
    QSlider *power;
    QSlider *radius;
    QLabel *label;
    QLabel *label_3;
    QCheckBox *ssao_map;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(800, 600);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        view = new View(centralWidget);
        view->setObjectName(QStringLiteral("view"));

        horizontalLayout->addWidget(view);

        MainWindow->setCentralWidget(centralWidget);
        controls = new QDockWidget(MainWindow);
        controls->setObjectName(QStringLiteral("controls"));
        controls->setMinimumSize(QSize(200, 89));
        control_objects = new QWidget();
        control_objects->setObjectName(QStringLiteral("control_objects"));
        control_objects->setMinimumSize(QSize(50, 50));
        shadows = new QCheckBox(control_objects);
        shadows->setObjectName(QStringLiteral("shadows"));
        shadows->setGeometry(QRect(10, 20, 87, 20));
        ambient_occlusion = new QCheckBox(control_objects);
        ambient_occlusion->setObjectName(QStringLiteral("ambient_occlusion"));
        ambient_occlusion->setGeometry(QRect(10, 60, 181, 20));
        power = new QSlider(control_objects);
        power->setObjectName(QStringLiteral("power"));
        power->setGeometry(QRect(20, 170, 160, 16));
        power->setCursor(QCursor(Qt::ArrowCursor));
        power->setAutoFillBackground(false);
        power->setInputMethodHints(Qt::ImhNone);
        power->setMinimum(0);
        power->setMaximum(100);
        power->setValue(10);
        power->setTracking(true);
        power->setOrientation(Qt::Horizontal);
        power->setInvertedAppearance(false);
        power->setTickPosition(QSlider::NoTicks);
        power->setTickInterval(1);
        radius = new QSlider(control_objects);
        radius->setObjectName(QStringLiteral("radius"));
        radius->setGeometry(QRect(20, 220, 160, 16));
        radius->setMinimum(5);
        radius->setMaximum(100);
        radius->setValue(10);
        radius->setSliderPosition(10);
        radius->setOrientation(Qt::Horizontal);
        label = new QLabel(control_objects);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(70, 150, 84, 21));
        label_3 = new QLabel(control_objects);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(70, 200, 84, 21));
        ssao_map = new QCheckBox(control_objects);
        ssao_map->setObjectName(QStringLiteral("ssao_map"));
        ssao_map->setGeometry(QRect(10, 100, 101, 27));
        controls->setWidget(control_objects);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), controls);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Welcome to the Museum!", nullptr));
        shadows->setText(QApplication::translate("MainWindow", "Shadows", nullptr));
        ambient_occlusion->setText(QApplication::translate("MainWindow", "Ambient Occlusion", nullptr));
        label->setText(QApplication::translate("MainWindow", "Power", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "Radius", nullptr));
        ssao_map->setText(QApplication::translate("MainWindow", "SSAO Map", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
