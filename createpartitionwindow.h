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
    void writeInWindow(int free_space);
    ~CreatePartition();

private slots:
    void on_cancelButton_clicked();
    void on_okButton_clicked();

signals:
    okButtonClicked(int size, QString name);

private:
    Ui::CreatePartition *ui;

    int free_space;
};

#endif // CREATEPARTITIONWINDOW_H
