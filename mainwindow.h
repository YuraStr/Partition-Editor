#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "harddiskmanager.h"
#include "createpartitionwindow.h"
#include "deletepartitionwindow.h"
#include "resizepartitionwindow.h"
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
    void setInfo();
    ~MainWindow();

private slots:
    void on_createButton_clicked();
    void createPartition(int size, QString name);
    void deletePartition();

    void on_deleteButton_clicked();

    void on_resizeButton_clicked();

private:
    Ui::MainWindow *ui;
    CreatePartition *createWindow;
    DeletePartitionWindow *deleteWindow;
    ResizePartitionWindow *resizeWindow;
};

#endif // MAINWINDOW_H
