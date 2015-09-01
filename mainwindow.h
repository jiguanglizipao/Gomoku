#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gamescene.h"
#include "gamesolver.h"
#include <QTcpSocket>
#include <QTimer>

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
    void timeout();

    void connectError(QAbstractSocket::SocketError error);

    void connected();

    void getData();

    void on_connectButton_clicked();

    void on_pauseButton_clicked();

    void on_startButton_clicked();

    void on_newButton_clicked();

    void on_stopButton_clicked();

    void on_undoButton_clicked();

    void on_saveButton_clicked();

    void on_loadButton_clicked();

public slots:
    void exchange(int x);

private:
    GameScene *gameScene;
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QString socketData;
    QTimer *timer;
    int timeNum;
};

#endif // MAINWINDOW_H
