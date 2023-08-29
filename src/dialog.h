#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_pushButton_clicked();
    void printNumber();
    float getDistance();
    void printDistance();

    void on_progressBar_valueChanged();

private:
    Ui::Dialog *ui;
    QTimer *timer;
    QString number;
};

#endif // DIALOG_H
