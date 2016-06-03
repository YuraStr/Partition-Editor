#ifndef DELETEPARTITIONWINDOW_H
#define DELETEPARTITIONWINDOW_H

#include <QDialog>

namespace Ui {
class DeletePartitionWindow;
}

class DeletePartitionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DeletePartitionWindow(QWidget *parent = 0);
    ~DeletePartitionWindow();

private slots:
    void on_noButton_clicked();

    void on_yesButton_clicked();

signals:
    deletePartition();

private:
    Ui::DeletePartitionWindow *ui;
};

#endif // DELETEPARTITIONWINDOW_H
