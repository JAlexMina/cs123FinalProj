#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "view.h"
#include <QGLFormat>
#include <QSettings>
#include <Settings.h>
#include <QKeyEvent>
#include <iostream>
#include "gl/GLDebug.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set up settings
    settings.use_ambient_occlusion = ui->ambient_occlusion->isChecked();
    settings.use_shadows = ui->shadows->isChecked();
    settings.cameraFar = 100.0f;
    settings.cameraNear = .1f;
    settings.cameraFov = .5;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_shadows_clicked()
{
    settings.use_shadows = ui->shadows->isChecked();
}

void MainWindow::on_ambient_occlusion_clicked()
{
    settings.use_ambient_occlusion = ui->ambient_occlusion->isChecked();
}

void MainWindow::on_power_valueChanged(int value)
{
    settings.power = float(value) / float(10.0);
}

void MainWindow::on_kernelSize_valueChanged(int value)
{
    settings.kernel_size = value;
}

void MainWindow::on_radius_valueChanged(int value)
{
    settings.radius = float(value) / float(20.0);
}

void MainWindow::on_ssao_map_clicked()
{
    settings.display_ssao_map = ui->ssao_map->isChecked();
}
