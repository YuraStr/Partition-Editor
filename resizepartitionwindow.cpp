#include "resizepartitionwindow.h"
#include "ui_resizepartitionwindow.h"

ResizePartitionWindow::ResizePartitionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResizePartitionWindow)
{
    ui->setupUi(this);

    setWindowTitle("Resize partition");

    free_space_behind = 0;
    free_space_partition = 0;
    current_size = 0;

    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(spinBoxChanged(int)));
}

void ResizePartitionWindow::writeIn(int free_space_partition, int free_space_behind, int current_size)
{
    this->free_space_partition = free_space_partition;
    this->free_space_behind = free_space_behind;
    this->current_size = current_size;

    ui->spinBox->setMaximum(this->current_size + this->free_space_behind);
    ui->spinBox->setMinimum(this->current_size - this->free_space_partition);
    ui->spinBox->setValue(this->current_size);
}

ResizePartitionWindow::~ResizePartitionWindow()
{
    delete ui;
}

void ResizePartitionWindow::on_pushButton_2_clicked()
{
    close();
}

void ResizePartitionWindow::spinBoxChanged(int value)
{
    ui->partSpace->setText(QString::number(free_space_partition - (current_size - value)));
    ui->behindSpace->setText(QString::number(free_space_behind + (current_size - value)));
}

void ResizePartitionWindow::on_pushButton_clicked()
{
    emit resizePartition(ui->spinBox->text().toInt());
    close();
}
