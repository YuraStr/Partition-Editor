#ifndef CREATEPARTITIONWINDOW_H
#define CREATEPARTITIONWINDOW_H

#include <QDialog>

namespace Ui {
class CreatePartition;
}

class CreatePartition : public QDialog
{
    Q_OBJECT

public:
    explicit CreatePartition(QWidget *parent = 0);
    ~CreatePartition();

private slots:
    void on_cancelButton_clicked();

private:
    Ui::CreatePartition *ui;
};

#endif // CREATEPARTITIONWINDOW_H
