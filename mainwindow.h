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
    void createPartition(int size);
    void deletePartition();
    void resizePartition(int new_size);

    void on_createButton_clicked();
    
    void on_deleteButton_clicked();

    void on_resizeButton_clicked();

private:
    Ui::MainWindow *ui;
    CreatePartitionWindow *createWindow;
    DeletePartitionWindow *deleteWindow;
    ResizePartitionWindow *resizeWindow;
};

#endif // MAINWINDOW_H
