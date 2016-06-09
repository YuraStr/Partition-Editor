#include "CreatePartitionWindow.h"
#include "ui_CreatePartitionWindow.h"

CreatePartitionWindow::CreatePartitionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreatePartitionWindow)
{
    ui->setupUi(this);

    setWindowTitle("Create partition");
}

CreatePartitionWindow::~CreatePartitionWindow()
{
    delete ui;
}

void CreatePartitionWindow::on_cancelButton_clicked()
{
    close();
}

void CreatePartitionWindow::writeInWindow(int free_space)
{
    this->free_space = free_space;
    ui->freeSpaceLabel->setText(QString::number(free_space));
}

void CreatePartitionWindow::on_okButton_clicked()
{
    emit okButtonClicked(ui->sizeLineEdit->text().toInt());
    close();
}

void CreatePartitionWindow::on_sizeLineEdit_textChanged(const QString &arg1)
{
    ui->freeSpaceLabel->setText(QString::number(free_space - arg1.toInt()));
    if (ui->sizeLineEdit->text().toInt() > free_space)
        ui->sizeLineEdit->setText(QString::number(free_space));
}
