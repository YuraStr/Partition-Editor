#include "resizepartitionwindow.h"
#include "ui_resizepartitionwindow.h"

ResizePartitionWindow::ResizePartitionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResizePartitionWindow)
{
    ui->setupUi(this);

    ui->spinBox->setMaximum(1000000);
}

ResizePartitionWindow::~ResizePartitionWindow()
{
    delete ui;
}

void ResizePartitionWindow::on_pushButton_2_clicked()
{
    close();
}
