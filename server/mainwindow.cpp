#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QLabel>
#include <QVariant>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    map(this)
{
    ui->setupUi(this);
    server = new QTcpServer(this);
    QTimer *timer = new QTimer(this), *timer2 = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(changeList()));
    timer->start(5000);
    connect(timer2, SIGNAL(timeout()), this, SLOT(readyRead()));
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newConnection()
{
    QTcpSocket *tmp = server->nextPendingConnection();
    if(!tmp)return;
    qDebug()<<tmp->peerAddress()<<" "<<tmp->peerPort()<<" "<<tmp->peerName();
    socket.push_back(tmp);
    name.push_back("");
    socketItem.push_back(0);
    connect(tmp, SIGNAL(disconnected()), this, SLOT(removeConnection()));
    connect(tmp, SIGNAL(readyRead()), &map, SLOT(map()));

    QVariant port(tmp->peerPort());
    map.setMapping(tmp, tmp->peerAddress().toString()+":"+port.toString());
    connect(&map, SIGNAL(mapped(QString)), this, SLOT(readyRead(QString)));

//    socketItem.push_back(new QListWidgetItem(tmp->peerName()+"  "+tmp->peerAddress().toString()+":"+port.toString(), ui->ipList));
//    ui->ipList->addItem(socketItem.back());
}

void MainWindow::readyRead(QString str)
{
    qDebug()<<str;
    for(int i=0;i<socket.size();i++)
    {
        if(str == (socket[i]->peerAddress().toString()+":"+QString::number(socket[i]->peerPort())) || str.isEmpty())
        {
            QString socketData = QString::fromLocal8Bit(socket[i]->readAll());
            QTextStream ss(&socketData);
            QString ind = ss.readLine();
            qDebug()<<ind;
            if(ind == "Gomoku Set Nickname")
            {
                QString tmp = ss.readLine();
                if(str.isEmpty())
                {
                    socket[i]->write(QString("Gomoku Set Nickname Reject\n").toLocal8Bit());
                    return;
                }
                for(int j=0;j<socket.size();j++)if(name[j] == tmp)
                {
                    socket[i]->write(QString("Gomoku Set Nickname Reject\n").toLocal8Bit());
                    return;
                }
                name[i] = tmp;
                socketItem[i] = new QListWidgetItem(name[i]+"  "+socket[i]->peerAddress().toString()+":"+QString::number(socket[i]->peerPort()), ui->ipList);
                ui->ipList->addItem(socketItem[i]);
                changeList();
                break;
            }
            if(ind == "Gomoku Chat")
            {
                for(int j=0;j<socket.size();j++)if(i!=j)
                {
                    socket[j]->write(socketData.toLocal8Bit());
                }
                break;
            }
            if(ind == "Gomoku Play With")
            {
                QString tmp = ss.readLine();
                if(name[i] == tmp)
                {
                    socket[i]->write(QString("Gomoku Play With Reject\n%1").arg(tmp).toLocal8Bit());
                    return;
                }
                for(int j=0;j<game.size();j++)
                {
                    if(game[j].first == tmp || game[j].second == tmp || game[j].first == name[i] || game[j].second == name[i])
                    {
                        socket[i]->write(QString("Gomoku Play With Reject\n%1").arg(tmp).toLocal8Bit());
                        return;
                    }
                }
                for(int j=0;j<name.size();j++)if(tmp == name[j])
                {
                    game.push_back(qMakePair(name[i], tmp));
                    socket[j]->write(QString("Gomoku Play With\n%1").arg(name[i]).toLocal8Bit());
                    return;
                }
                break;
            }
            if(ind == "Gomoku Play With Reject")
            {
                QString tmp = ss.readLine();
                for(int j=0;j<game.size();j++)
                {
                    if(game[j].first == tmp || game[j].second == tmp || game[j].first == name[i] || game[j].second == name[i])
                    {
                        game.removeAt(j);
                        return;
                    }
                }
                for(int j=0;j<name.size();j++)if(tmp == name[j])
                {
                    socket[j]->write(socketData.toLocal8Bit());
                    return;
                }
                break;
            }
            if(ind == "Gomoku Play With Disconnect")
            {
                QString tmp;
                for(int j=0;j<game.size();j++)
                    if(game[j].first == name[i] || game[j].second == name[i])
                    {
                        if(game[j].first == name[i])tmp=game[j].second;
                        if(game[j].second == name[i])tmp=game[j].first;
                        game.removeAt(j);
                        break;
                    }
                for(int j=0;j<name.size();j++)if(tmp == name[j])
                {
                    socket[j]->write(socketData.toLocal8Bit());
                    return;
                }
                break;
            }
            for(int j=0;j<game.size();j++)
            {
                QString tmp;
                if(game[j].first == name[i])tmp=game[j].second;
                if(game[j].second == name[i])tmp=game[j].first;
                for(int j=0;j<name.size();j++)if(tmp == name[j])
                {
                    socket[j]->write(socketData.toLocal8Bit());
                    return;
                }
            }
            break;
        }
    }
}

void MainWindow::removeConnection()
{
    for(int i=0;i<socket.size();i++)
    {
        if(socket[i]->state() == QTcpSocket::UnconnectedState){
            if(!name[i].isEmpty())
            {
                for(int j=0;j<game.size();j++)
                {
                    QString tmp;
                    if(game[j].first == name[i])tmp=game[j].second;
                    if(game[j].second == name[i])tmp=game[j].first;
                    for(int j=0;j<name.size();j++)if(tmp == name[j])
                    {
                        socket[j]->write(QString("Gomoku Play With Disconnect\n").toLocal8Bit());
                        break;
                    }
                }
                ui->ipList->removeItemWidget(socketItem[i]);
                delete socketItem[i];
            }
            name.removeAt(i);
            socketItem.removeAt(i);
            socket.removeAt(i);
            break;
        }
    }
    changeList();
}

void MainWindow::on_listenButton_clicked()
{
    if(server->isListening())server->close();
    server->listen(QHostAddress::Any, ui->portBox->value());
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void MainWindow::changeList()
{
    QString tmp = "Gomoku Client List\n";
    for(int i=0;i<socket.size();i++)if(!name[i].isEmpty())tmp+=name[i]+"\n";
    for(int i=0;i<socket.size();i++)socket[i]->write(tmp.toLocal8Bit());
}
