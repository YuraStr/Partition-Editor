#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createWindow = new CreatePartitionWindow();
    deleteWindow = new DeletePartitionWindow();
    resizeWindow = new ResizePartitionWindow();
    hdm = new HardDiskManager();

    setInfo();

    connect(createWindow, SIGNAL(okButtonClicked(int)), this, SLOT(createPartition(int)));
    connect(deleteWindow, SIGNAL(deletePartition()), this, SLOT(deletePartition()));
    connect(resizeWindow, SIGNAL(resizePartition(int)), this, SLOT(resizePartition(int)));
}

void MainWindow::setInfo()
{
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->setRowCount(hdm->getPartitionCount());
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Size (MB)"));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("File system"));
    ui->tableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem("Type"));
    ui->tableWidget->setHorizontalHeaderItem(4, new QTableWidgetItem("Free space (MB)"));

    if (hdm->getPartitionCount() == -1) {
    } else {
        if (ui->tableWidget->selectedItems().size() > 4) {
            QMessageBox msg;
            msg.setText("Error");
        }

        for (int i = 0; i < hdm->getPartitionCount(); i++) {
            PartitionStructure partition = hdm->getPartition(i);
            ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem("Partition " + QString::number(i + 1)));
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::fromLatin1(partition.name)));
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(partition.partitionInformation.PartitionLength.QuadPart / 1024 / 1024)));
            ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromLatin1(partition.file_system)));
            if (partition.partitionInformation.PartitionStyle == PARTITION_STYLE_RAW)
                ui->tableWidget->setItem(i, 3, new QTableWidgetItem("Empty space"));
            if (partition.partitionInformation.PartitionStyle == PARTITION_STYLE_MBR)
                ui->tableWidget->setItem(i, 3, new QTableWidgetItem("MBR"));
            if (partition.partitionInformation.PartitionStyle == PARTITION_STYLE_GPT)
                ui->tableWidget->setItem(i, 3, new QTableWidgetItem("GPT"));
            ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(partition.free_space.QuadPart / 1024 / 1024)));
        }
    }
}

MainWindow::~MainWindow()
{
    delete hdm;
    delete createWindow;
    delete deleteWindow;
    delete resizeWindow;
    delete ui;
}

void MainWindow::on_createButton_clicked()
{
    if (strcmp(ui->tableWidget->selectedItems().at(3)->text().toStdString().c_str(), "Empty space")) {
        QMessageBox msg;
        msg.setText("Error: choose empty partition!");
        msg.exec();
        return;
    }

    createWindow->writeInWindow(ui->tableWidget->selectedItems().at(4)->text().toInt());
    createWindow->exec();
}

void MainWindow::createPartition(int size)
{
    hdm->createNewPartition(ui->tableWidget->row(ui->tableWidget->selectedItems().at(0)) + 1, size);
    ui->tableWidget->clear();
    setInfo();
}

void MainWindow::on_deleteButton_clicked()
{
    deleteWindow->exec();
}

void MainWindow::deletePartition()
{
    hdm->deletePartition(ui->tableWidget->row(ui->tableWidget->selectedItems().at(0)) + 1);
    ui->tableWidget->clear();
    setInfo();
}

void MainWindow::on_resizeButton_clicked()
{
    int number = ui->tableWidget->row(ui->tableWidget->selectedItems().at(0));
    if (hdm->getPartition(number + 1).isEmptySpace) {
        resizeWindow->writeIn(ui->tableWidget->selectedItems().at(4)->text().toInt(),
                              hdm->getPartition(number + 1).free_space.QuadPart / pow(1024, 2),
                              ui->tableWidget->selectedItems().at(1)->text().toInt());
    } else {
        resizeWindow->writeIn(ui->tableWidget->selectedItems().at(4)->text().toInt(),
                              0,
                              ui->tableWidget->selectedItems().at(1)->text().toInt());
    }
    resizeWindow->exec();
}

void MainWindow::resizePartition(int new_size)
{
    hdm->changePartitionSize(ui->tableWidget->row(ui->tableWidget->selectedItems().at(0)) + 1, new_size);
    ui->tableWidget->clear();
    setInfo();
}
