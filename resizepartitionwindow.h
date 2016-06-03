#ifndef RESIZEPARTITIONWINDOW_H
#define RESIZEPARTITIONWINDOW_H

#include <QDialog>

namespace Ui {
class ResizePartitionWindow;
}

class ResizePartitionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ResizePartitionWindow(QWidget *parent = 0);
    ~ResizePartitionWindow();

private slots:
    void on_pushButton_2_clicked();

private:
    Ui::ResizePartitionWindow *ui;
};

#endif // RESIZEPARTITIONWINDOW_H
