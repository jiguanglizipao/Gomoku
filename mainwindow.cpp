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
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    gameScene = new GameScene(this);
    ui->gameView->setScene(gameScene);
    connect(gameScene, SIGNAL(exchange(int)), this, SLOT(exchange(int)));

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(getData()));

    server = new QTcpServer(this);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(getData()));

    timer = new QTimer(this);
    timeNum =  0;
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));

}

void MainWindow::exchange(int x)
{
    ui->undoButton->setEnabled(true);
    if(gameScene->type == 0)ui->player1Time->display(0);
    if(gameScene->type == 1)ui->player2Time->display(0);
    timer->stop();
    timer->start(100);
    this->timeNum = 0;
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
}

void MainWindow::newConnection()
{
    socket = server->nextPendingConnection();
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(getData()));
    ui->newButton->setEnabled(true);
}

void MainWindow::timeout()
{
    timeNum++;
    if(gameScene->type == 0)ui->player1Time->display(20.0-double(timeNum)/10);
    if(gameScene->type == 1)ui->player2Time->display(20.0-double(timeNum)/10);
    if(timeNum >=  200)
    {
        QMessageBox::information(this, "Timeout", QString("Player%1 Win!").arg(1+(gameScene->type^1)));
        on_newButton_clicked();
        timer->stop();
    }

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
    if(ind == "Gomoku New Game")
    {
        int x;ss>>x;
        if(QMessageBox::question(this, QString("New Game"), QString("Create a new game?")) != QMessageBox::Yes){
            socket->write("Gomoku New Game Reject");
            return;
        }
        gameScene->type = x;
        flag.New = 2;
        socket->write("Gomoku New Game Accept");
        on_newButton_clicked();

    }
    if(ind == "Gomoku New Game Reject")
    {
        flag.New = 0;
    }
    if(ind == "Gomoku New Game Accept")
    {
        flag.New = 2;
        on_newButton_clicked();
    }
    if(ind == "Gomoku Stop Game")
    {
    }
    if(ind == "Gomoku Start Game")
    {
    }
    if(ind == "Gomoku Pause Game")
    {
    }
    if(ind == "Gomoku Undo Game")
    {
    }
    if(ind == "Gomoku Load Game")
    {
    }
    if(ind == "Gomoku Save Game")
    {
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pauseButton_clicked()
{
    gameScene->disable=true;
    ui->startButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    ui->saveButton->setEnabled(true);
    ui->loadButton->setEnabled(true);
    gameScene->drawMap();
    timer->stop();
}

void MainWindow::on_startButton_clicked()
{
    gameScene->disable=false;
    ui->startButton->setEnabled(false);
    ui->pauseButton->setEnabled(true);
    ui->saveButton->setEnabled(false);
    ui->loadButton->setEnabled(false);
    gameScene->drawMap();
    timer->start(100);
}

void MainWindow::on_newButton_clicked()
{

    switch(flag.New)
    {
    case 2:
        gameScene->Init();
        ui->newButton->setEnabled(true);
        ui->startButton->setEnabled(true);
        ui->loadButton->setEnabled(true);
        ui->saveButton->setEnabled(true);
        ui->startButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->undoButton->setEnabled(false);
        timer->stop();
        this->timeNum = 0;
        ui->player1Time->display(0);
        ui->player2Time->display(0);
        flag.New = 0;
        break;
    case 0:
        flag.New = 1;
        gameScene->type = qrand()%2;
        QString tmp = QString("Gomoku New Game\n%1").arg(gameScene->type^1);
        socket->write(tmp.toLocal8Bit());
        break;
    }
}

void MainWindow::on_stopButton_clicked()
{
    on_newButton_clicked();
    QMessageBox::information(this, QString("Game Stop"), QString("Player%1 Win!").arg(1+(gameScene->type^1)));
}

void MainWindow::on_undoButton_clicked()
{
    ui->undoButton->setDisabled(gameScene->Undo());
    this->timeNum = 0;
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
    on_startButton_clicked();
    file.write(gameScene->toString().toLocal8Bit());
    file.close();
}

void MainWindow::on_loadButton_clicked()
{
    timer->stop();
    QString fileName;
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
    on_startButton_clicked();
    on_pauseButton_clicked();
    gameScene->fromString(file.readAll());
    file.close();
}

