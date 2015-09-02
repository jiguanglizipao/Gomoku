#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QSignalMapper>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    flag()
{
    ui->setupUi(this);
    gameScene = new GameScene(this);
    ui->gameView->setScene(gameScene);
    connect(gameScene, SIGNAL(moveChess(int, int)), this, SLOT(moveChess(int, int)));
    connect(gameScene, SIGNAL(gameEnd(int)), this, SLOT(gameEnd(int)));

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(getData()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    server = new QTcpServer(this);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(getData()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    webTimer = new QTimer(this);
    connect(webTimer, SIGNAL(timeout()), this, SLOT(getData()));

}

void MainWindow::moveChess(int x, int y)
{
    ui->undoButton->setEnabled(true);
    QString tmp = QString("Gomoku Move Chess\n%1 %2").arg(x).arg(y);
    qDebug()<<tmp;
    socket->write(tmp.toLocal8Bit());
    if(gameScene->type == 0)ui->player2Time->display(20);
    if(gameScene->type == 1)ui->player1Time->display(20);
}

void MainWindow::gameEnd(int x)
{
    timer->stop();
    if(x == gameScene->type)
        QMessageBox::information(this, QString("Game End"), QString("You Win!"));
    else
        QMessageBox::information(this, QString("Game End"), QString("You Lose!"));
    ui->pauseButton->setEnabled(false);
    ui->stopButton->setEnabled(false);
    ui->undoButton->setEnabled(false);
    gameScene->disable = true;
    gameScene->drawMap();
}

void MainWindow::connectError(QAbstractSocket::SocketError error)
{
    QMessageBox::critical(this, tr("ERROR!"), socket->errorString());
    if(error != QAbstractSocket::SocketTimeoutError){
        socket->disconnectFromHost();
    }
}

void MainWindow::connected()
{
    QMessageBox::information(this, "Connected", "Connected to server successfuly");
    ui->connectButton->setDisabled(true);
    ui->listenButton->setDisabled(true);
    ui->newButton->setEnabled(true);
    webTimer->start(1000);
}

void MainWindow::disconnected()
{
}

void MainWindow::newConnection()
{
    socket = server->nextPendingConnection();
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(getData()));
    ui->newButton->setEnabled(true);
    webTimer->start(1000);
}

void MainWindow::timeout(bool f)
{
    if(f && !gameScene->move)return;
    if(f)
    {
        QString tmp = QString("Gomoku Timer Timeout");
        socket->write(tmp.toLocal8Bit());
        if(gameScene->type == 0)
            ui->player1Time->display(ui->player1Time->value()-0.1);
        else
            ui->player2Time->display(ui->player2Time->value()-0.1);
    }
    else
    {
        if(gameScene->type == 0)
            ui->player2Time->display(ui->player2Time->value()-0.1);
        else
            ui->player1Time->display(ui->player1Time->value()-0.1);

    }
    if(ui->player1Time->value() < 0)gameEnd(1);
    if(ui->player2Time->value() < 0)gameEnd(0);
}


void MainWindow::on_connectButton_clicked()
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(getData()));
    socket->connectToHost(ui->ipEdit->text(), ui->portBox->value());
}


void MainWindow::on_listenButton_clicked()
{
    server = new QTcpServer(this);
    server->listen(QHostAddress::Any, ui->portBox->value());
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    ui->connectButton->setDisabled(true);
    ui->listenButton->setDisabled(true);
}

void MainWindow::getData()
{
    socketData = socket->readAll();
    QTextStream ss(&socketData);
    QString ind = ss.readLine();
    qDebug()<<ind;
    if(ind == "Gomoku New Game")
    {
        timer->stop();
        if(QMessageBox::question(this, QString("New Game"), QString("Create a new game?")) != QMessageBox::Yes){
            socket->write("Gomoku New Game Reject");
            timer->start(100);
            return;
        }
        gameScene->type = qrand()%2;
        flag.New = 2;
        socket->write(QString("Gomoku New Game Accept\n%1").arg(gameScene->type^1).toLocal8Bit());
        QMessageBox::information(this, QString("New Game"), QString("You are player%1").arg(gameScene->type+1));
        socket->write(QString("Gomoku New Game Confirm\n").toLocal8Bit());
    }

    if(ind == "Gomoku New Game Confirm")
    {
        on_newButton_clicked();
    }

    if(ind == "Gomoku New Game Reject")
    {
        timer->start(100);
        flag.New = 0;
        ui->newButton->setEnabled(true);
    }
    if(ind == "Gomoku New Game Accept")
    {
        ss>>gameScene->type;
        flag.New = 2;
        QMessageBox::information(this, QString("New Game"), QString("You are player%1").arg(gameScene->type+1));
        socket->write(QString("Gomoku New Game Confirm\n").toLocal8Bit());
    }


    if(ind == "Gomoku Stop Game")
    {
        timer->stop();
        if(QMessageBox::question(this, QString("Stop Game"), QString("Stop this game?")) != QMessageBox::Yes){
            socket->write("Gomoku Stop Game Reject");
            timer->start();
            return;
        }
        flag.Stop = 2;
        socket->write(QString("Gomoku Stop Game Accept").toLocal8Bit());
        QMessageBox::information(this, QString("Stop Game"), QString("You Win!"));
        on_stopButton_clicked();

    }
    if(ind == "Gomoku Stop Game Reject")
    {
        timer->start();
        flag.Stop = 0;
        ui->stopButton->setEnabled(true);
    }
    if(ind == "Gomoku Stop Game Accept")
    {
        flag.Stop = 2;
        QMessageBox::information(this, QString("Stop Game"), QString("You Lose!"));
        on_stopButton_clicked();
    }


    if(ind == "Gomoku Start Game")
    {
        timer->stop();
        if(QMessageBox::question(this, QString("Start Game"), QString("Start this game?")) != QMessageBox::Yes){
            socket->write("Gomoku Start Game Reject");
            return;
        }
        flag.Start = 2;
        socket->write(QString("Gomoku Start Game Accept").toLocal8Bit());
        on_startButton_clicked();

    }
    if(ind == "Gomoku Start Game Reject")
    {
        flag.Start = 0;
        ui->startButton->setEnabled(true);
    }
    if(ind == "Gomoku Start Game Accept")
    {
        flag.Start = 2;
        on_startButton_clicked();
    }

    if(ind == "Gomoku Pause Game")
    {
        timer->stop();
        if(QMessageBox::question(this, QString("Pause Game"), QString("Pause this game?")) != QMessageBox::Yes){
            socket->write("Gomoku Pause Game Reject");
            timer->start(100);
            return;
        }
        flag.Pause = 2;
        socket->write(QString("Gomoku Pause Game Accept").toLocal8Bit());
        on_pauseButton_clicked();

    }
    if(ind == "Gomoku Pause Game Reject")
    {
        flag.Pause = 0;
        timer->start(100);
        ui->pauseButton->setEnabled(true);
    }
    if(ind == "Gomoku Pause Game Accept")
    {
        flag.Pause = 2;
        on_pauseButton_clicked();
    }

    if(ind == "Gomoku Undo Game")
    {
        timer->stop();
        if(QMessageBox::question(this, QString("Undo Game"), QString("Undo this game?")) != QMessageBox::Yes){
            socket->write("Gomoku Undo Game Reject");
            timer->start(100);
            return;
        }
        flag.Undo = 2;
        socket->write(QString("Gomoku Undo Game Accept").toLocal8Bit());
        on_undoButton_clicked(gameScene->type^1);

    }
    if(ind == "Gomoku Undo Game Reject")
    {
        timer->start(100);
        flag.Undo = 0;
        ui->undoButton->setEnabled(true);
    }
    if(ind == "Gomoku Undo Game Accept")
    {
        flag.Undo = 2;
        on_undoButton_clicked(gameScene->type);
    }

    if(ind == "Gomoku Load Game")
    {
        timer->stop();
        if(QMessageBox::question(this, QString("Load Game"), QString("Load a new game?")) != QMessageBox::Yes){
            socket->write("Gomoku Load Game Reject");
            timer->start(100);
            return;
        }
        flag.Load = 2;
        socket->write(QString("Gomoku Load Game Accept").toLocal8Bit());
        timer->stop();
        gameScene->disable=true;
        ui->startButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->saveButton->setEnabled(true);
        ui->loadButton->setEnabled(true);
    }
    if(ind == "Gomoku Load Game Reject")
    {
        timer->start(100);
        flag.Load = 0;
        ui->undoButton->setEnabled(true);
    }
    if(ind == "Gomoku Load Game Accept")
    {
        flag.Load = 2;
        on_loadButton_clicked();
    }
    if(ind == "Gomoku Save Data")
    {
        gameScene->fromString(socketData);
    }


    if(ind == "Gomoku Move Chess")
    {
        int x, y;
        ss>>x>>y;
        gameScene->setChess(x, y);
        if(gameScene->type == 0)ui->player1Time->display(20);
        if(gameScene->type == 1)ui->player2Time->display(20);
    }

    if(ind == "Gomoku Timer Timeout")
    {
        timeout(false);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pauseButton_clicked()
{
    switch(flag.Pause)
    {
    case 2:
        ui->pauseButton->setEnabled(true);

        gameScene->disable=true;
        ui->startButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->saveButton->setEnabled(true);
        ui->loadButton->setEnabled(true);
        gameScene->drawMap();
        timer->stop();

        flag.Pause = 0;
        break;
    case 0:
        flag.Pause = 1;
        QString tmp = QString("Gomoku Pause Game");
        socket->write(tmp.toLocal8Bit());
        ui->pauseButton->setEnabled(false);
        timer->stop();
        break;
    }

}

void MainWindow::on_startButton_clicked()
{
    switch(flag.Start)
    {
    case 2:
        ui->startButton->setEnabled(true);

        gameScene->disable=false;
        ui->startButton->setEnabled(false);
        ui->pauseButton->setEnabled(true);
        ui->saveButton->setEnabled(false);
        ui->loadButton->setEnabled(false);
        gameScene->drawMap();
        timer->start(100);
        flag.Start = 0;
        break;
    case 0:
        flag.Start = 1;
        QString tmp = QString("Gomoku Start Game");
        socket->write(tmp.toLocal8Bit());
        ui->startButton->setEnabled(false);
        timer->stop();
        break;
    }


}

void MainWindow::on_newButton_clicked()
{

    switch(flag.New)
    {
    case 2:
        gameScene->Init();
        ui->newButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
        ui->startButton->setEnabled(true);
        ui->loadButton->setEnabled(true);
        ui->saveButton->setEnabled(true);
        ui->startButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->undoButton->setEnabled(false);

        gameScene->move = (gameScene->type == 0);
        timer->stop();
        ui->player1Time->display(20);
        ui->player2Time->display(20);
        flag.New = 0;
        break;
    case 0:
        flag.New = 1;
        ui->newButton->setEnabled(false);
        QString tmp = QString("Gomoku New Game");
        socket->write(tmp.toLocal8Bit());
        timer->stop();
        break;
    }
}

void MainWindow::on_stopButton_clicked()
{
    switch(flag.Stop)
    {
    case 2:
        gameScene->disable = true;
        ui->newButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
        ui->startButton->setEnabled(false);
        ui->loadButton->setEnabled(false);
        ui->saveButton->setEnabled(true);
        ui->startButton->setEnabled(false);
        ui->pauseButton->setEnabled(false);
        ui->undoButton->setEnabled(false);
        timer->stop();
        ui->player1Time->display(0);
        ui->player2Time->display(0);
        flag.Stop = 0;
        break;
    case 0:
        flag.Stop = 1;
        ui->stopButton->setEnabled(false);
        QString tmp = QString("Gomoku Stop Game");
        socket->write(tmp.toLocal8Bit());
        timer->stop();
        break;
    }
}

void MainWindow::on_undoButton_clicked(int x)
{
    switch(flag.Undo)
    {
    case 2:
        ui->undoButton->setEnabled(true);
        ui->undoButton->setDisabled(gameScene->Undo(x));
        ui->player1Time->display(20);
        ui->player2Time->display(20);
        timer->start(100);

        flag.Undo = 0;
        break;
    case 0:
        flag.Undo = 1;
        ui->undoButton->setEnabled(false);
        QString tmp = QString("Gomoku Undo Game");
        socket->write(tmp.toLocal8Bit());
        timer->stop();
        break;
    }
}

void MainWindow::on_saveButton_clicked()
{
    timer->stop();
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, tr("Load Save"), "", tr("Save Files(*.sav);;All Files(*.*)"));
    if (fileName == ""){
        timer->start(100);
        return;
    }
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("ERROR!"), tr("Can not open this fils. %1").arg(fileName));
        return;
    }
    file.write(gameScene->toString().toLocal8Bit());
    file.close();
}

void MainWindow::on_loadButton_clicked()
{
    switch(flag.Load)
    {
    case 2:
    {
        ui->loadButton->setEnabled(true);
        timer->stop();
        QString fileName, tmp;
        fileName = QFileDialog::getOpenFileName(this, tr("Load Save"), "", tr("Save Files(*.sav);;All Files(*.*)"));
        if (fileName == ""){
            timer->start(100);
            return;
        }
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::critical(this, tr("ERROR!"), tr("Can not open this fils. %1").arg(fileName));
            return;
        }

        gameScene->disable=true;
        ui->startButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->saveButton->setEnabled(true);
        ui->loadButton->setEnabled(true);
        ui->player1Time->display(20);
        ui->player1Time->display(20);

        tmp = file.readAll();
        socket->write(tmp.toLocal8Bit());
        gameScene->fromString(tmp);
        file.close();
    }
        flag.Load = 0;
        break;
    case 0:
        flag.Load = 1;
        ui->loadButton->setEnabled(false);
        QString tmp = QString("Gomoku Load Game");
        socket->write(tmp.toLocal8Bit());
        timer->stop();
        break;
    }

}

