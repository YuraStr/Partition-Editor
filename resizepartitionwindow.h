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
    void writeIn(int free_space_partition, int free_space_behind, int current_size);
    ~ResizePartitionWindow();

private slots:
    void on_pushButton_2_clicked();
    void spinBoxChanged(int);

    void on_pushButton_clicked();

signals:
    resizePartition(int new_size);

private:
    Ui::ResizePartitionWindow *ui;

    int free_space_partition;
    int free_space_behind;
    int current_size;
};

#endif // RESIZEPARTITIONWINDOW_H
