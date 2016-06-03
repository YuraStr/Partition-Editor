#include "createpartitionwindow.h"
#include "ui_createpartitionwindow.h"

CreatePartition::CreatePartition(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreatePartition)
{
    ui->setupUi(this);


    QRegExp sizerx("[0-9]");
    QRegExp namerx("[A-Za-z]");


    ui->sizeLineEdit->setValidator(new QRegExpValidator(sizerx));
    ui->nameLineEdit->setValidator(new QRegExpValidator(namerx));

}

CreatePartition::~CreatePartition()
{
    delete ui;
}

void CreatePartition::on_cancelButton_clicked()
{
    close();
}

void CreatePartition::writeInWindow(int free_space)
{
    this->free_space = free_space;
    ui->freeSpaceLabel->setText(QString::number(free_space));
}

void CreatePartition::on_okButton_clicked()
{
    emit okButtonClicked(ui->sizeLineEdit->text().toInt(), ui->nameLineEdit->text());
    close();
}
