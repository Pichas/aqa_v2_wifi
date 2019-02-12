#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QScopedPointer>
#include <QDialog>
#include <QColorDialog>
#include <QTimer>
#include <QThread>
#include <QFile>
#include <QFileDialog>
#include <QDir>



#include "addtimerwin.h"
#include "timersmodeltable.h"
#include "driver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;


    void setIPPort();
    QScopedPointer<driver> device;

    void makeMatrix();
    void delMatrix();
    QMap<QPushButton*, QColor> allLeds;

    QColor brushColor;


private slots:
    void uploadToDevice();
    void loadStatus();
    void refreshActions();

    void exportTimers();
    void importTimers();

    void setLedColor();
    void pickBrushColor();

    void exportLedMap();
    void importLedMap();









signals:








};

#endif // MAINWINDOW_H
