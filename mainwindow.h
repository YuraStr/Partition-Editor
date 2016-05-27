#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "harddiskmanager.h"
#include "createpartition.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    HardDiskManager *hdm;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_createButton_clicked();

private:
    Ui::MainWindow *ui;
    CreatePartition *createWindow;
};

#endif // MAINWINDOW_H
