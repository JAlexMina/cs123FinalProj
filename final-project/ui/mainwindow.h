#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>

class View;

namespace Ui {
    class MainWindow;
}

/**
 * @class MainWindow
 *
 * The main graphical user interface class (GUI class) for our application.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_shadows_clicked();

    void on_ambient_occlusion_clicked();

    void on_power_valueChanged(int value);

    void on_kernelSize_valueChanged(int value);

    void on_radius_valueChanged(int value);

    void on_ssao_map_clicked();

private:
    // Auto-generated by Qt. DO NOT RENAME!
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H