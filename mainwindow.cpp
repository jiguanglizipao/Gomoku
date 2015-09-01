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
    socket->connectToHost(ui->ipEdit->text(), ui->portBox->value());
}

void MainWindow::getData()
{
    socketData = socket->readAll();
    qDebug()<<socketData;
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
    gameScene->Init();
    ui->startButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    timer->stop();
    this->timeNum = 0;
    ui->player1Time->display(0);
    ui->player2Time->display(0);
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
