#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QListWidgetItem>
#include <QSignalMapper>
#include <QPair>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void newConnection();
    void removeConnection();
    void on_listenButton_clicked();
    void readyRead(QString str="");
    void changeList();

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    QVector <QTcpSocket*> socket;
    QVector <QListWidgetItem*> socketItem;
    QVector <QString> name;
    QSignalMapper map;
    QVector <QPair<QString, QString> > game;

};

#endif // MAINWINDOW_H
