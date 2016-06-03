#include "deletepartitionwindow.h"
#include "ui_deletepartitionwindow.h"

DeletePartitionWindow::DeletePartitionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeletePartitionWindow)
{
    ui->setupUi(this);
}

DeletePartitionWindow::~DeletePartitionWindow()
{
    delete ui;
}

void DeletePartitionWindow::on_noButton_clicked()
{
    close();
}

void DeletePartitionWindow::on_yesButton_clicked()
{
    emit deletePartition();
    close();
}
