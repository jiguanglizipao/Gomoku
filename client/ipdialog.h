#ifndef IPDIALOG_H
#define IPDIALOG_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
class IPDialog;
}

class IPDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IPDialog(QWidget *parent = 0);
    QString ip;
    int port;
    ~IPDialog();

private:
    Ui::IPDialog *ui;
    QPushButton *button[13];
    void accept();

private slots:
    void input(QString str);
};

#endif // IPDIALOG_H
