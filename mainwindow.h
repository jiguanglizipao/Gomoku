#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gamescene.h"
#include "gamesolver.h"
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>

struct MessageFlag
{
    int New, Stop, Start, Pause, Load, Save, Undo;
};



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
    void timeout(bool f=true);

    void connectError(QAbstractSocket::SocketError error);

    void newConnection();

    void connected();

    void getData();

    void on_connectButton_clicked();

    void on_pauseButton_clicked();

    void on_startButton_clicked();

    void on_newButton_clicked();

    void on_stopButton_clicked();

    void on_undoButton_clicked(int x=0);

    void on_saveButton_clicked();

    void on_loadButton_clicked();

    void on_listenButton_clicked();

    void moveChess(int x, int y);

    void gameEnd(int x);

    void disconnected();

    void on_sendButton_clicked();

private:
    GameScene *gameScene;
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QTcpServer *server;
    QString socketData;
    QTimer *timer, *webTimer;
    MessageFlag flag;
    int player1, player2;
};

#endif // MAINWINDOW_H
