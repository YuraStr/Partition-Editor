#ifndef CreatePartitionWindowWINDOW_H
#define CreatePartitionWindowWINDOW_H

#include <QDialog>

namespace Ui {
class CreatePartitionWindow;
}

class CreatePartitionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CreatePartitionWindow(QWidget *parent = 0);
    void writeInWindow(int free_space);
    ~CreatePartitionWindow();

private slots:
    void on_cancelButton_clicked();
    void on_okButton_clicked();

    void on_sizeLineEdit_textChanged(const QString &arg1);

signals:
    okButtonClicked(int size);

private:
    Ui::CreatePartitionWindow *ui;

    int free_space;
};

#endif // CreatePartitionWindowWINDOW_H
