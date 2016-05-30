#include "createpartitionwindow.h"
#include "ui_createpartitionwindow.h"

CreatePartition::CreatePartition(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreatePartition)
{
    ui->setupUi(this);
}

CreatePartition::~CreatePartition()
{
    delete ui;
}

void CreatePartition::on_cancelButton_clicked()
{
    close();
}
