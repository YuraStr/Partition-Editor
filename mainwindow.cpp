#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createWindow = new CreatePartition();
    hdm = new HardDiskManager();
    if (hdm->getPartitionCount() == -1) {
        ui->label->setText("Error");
    } else {
        ui->label->setText(QString::number(hdm->getDiskSize().QuadPart / pow(1024, 2)));
        ui->tableWidget->setColumnCount(4);
        ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableWidget->setShowGrid(false);
        ui->tableWidget->setRowCount(hdm->getPartitionCount());
        ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Size (MB)"));
        ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Offset (MB)"));
        ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("Type"));
        ui->tableWidget->setColumnWidth(3, 209);

        if (ui->tableWidget->selectedItems().size() > 4) {
            QMessageBox msg;
            msg.setText("Error");
        }

        for (int i = 0; i < hdm->getPartitionCount(); i++) {
            PartitionStructure partition = hdm->getPartition(i);
            ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem("Partition " + QString::number(i + 1)));
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(partition.partitionInformation.PartitionLength.QuadPart / pow(1024, 2))));
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(partition.partitionInformation.StartingOffset.QuadPart / pow(1024, 2))));
            if (partition.partitionInformation.PartitionStyle == PARTITION_STYLE_RAW)
                ui->tableWidget->setItem(i, 2, new QTableWidgetItem("Empty space"));
            if (partition.partitionInformation.PartitionStyle == PARTITION_STYLE_MBR)
                ui->tableWidget->setItem(i, 2, new QTableWidgetItem("MBR"));
            if (partition.partitionInformation.PartitionStyle == PARTITION_STYLE_GPT)
                ui->tableWidget->setItem(i, 2, new QTableWidgetItem("GPT"));
        }
    }
}

MainWindow::~MainWindow()
{
    delete hdm;
    delete ui;
}

void MainWindow::on_createButton_clicked()
{
    if (strcmp(ui->tableWidget->selectedItems().at(2)->text().toStdString().c_str(), "Empty space")) {
        QMessageBox msg;
        msg.setText("Error: choose empty partition!");
        msg.show();
        return;
    }

    createWindow->exec();

}
