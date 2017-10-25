﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "PagePlotSingleCurve.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_newName(QString name);

    void on_delName(QString name);

    void on_timeout();

private slots:
    void on_actionTile_triggered();
    void on_actionCascade_triggered();
    void on_actionSave_triggered();

private:
    Ui::MainWindow *ui;
    QTimer mTimer;
};

#endif // MAINWINDOW_H
 