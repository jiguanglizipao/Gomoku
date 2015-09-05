#include "gameview.h"
#include <QDebug>
#include <QMouseEvent>
#include <QResizeEvent>
#include <iostream>

GameView::GameView(QWidget *parent)
    :QGraphicsView(parent)
{
    //this->setFixedSize(705, 705);
    //Scene->setSceneRect(0, 0, 700, 700);
}

void GameView::mousePressEvent(QMouseEvent  *event)
{
    //qDebug()<<"Press "<<event->button()<<" "<<event->pos().x()<<" "<<event->pos().y();
    QPointF tmp = mapToScene(event->pos());
    if(event->button() == 1)Scene->Press(tmp.x(), tmp.y());
    //Scene->Update(true, event->pos().x(), event->pos().y());
}

void GameView::mouseMoveEvent(QMouseEvent  *event)
{
    //qDebug()<<"Move "<<event->pos().x()<<" "<<event->pos().y();
    QPointF tmp = mapToScene(event->pos());
    Scene->Move(tmp.x(), tmp.y());
    //Scene->Update(true, event->pos().x(), event->pos().y());
}

void GameView::mouseReleaseEvent(QMouseEvent  *event)
{
    //qDebug()<<"Release "<<event->button()<<" "<<event->pos().x()<<" "<<event->pos().y();
    QPointF tmp = mapToScene(event->pos());
    if(event->button() == 1)Scene->Release(tmp.x(), tmp.y());
    //Scene->Update(false, event->pos().x(), event->pos().y());
}

void GameView::resizeEvent(QResizeEvent *event)
{
    int size = std::min(event->size().height(), event->size().width());
    Scene->setSceneRect(0, 0, size, size);
    Scene->Resize(size);
    QGraphicsView::resizeEvent(event);
}

void GameView::setScene(GameScene *scene)
{
    Scene = scene;
    QGraphicsView::setScene(scene);
}


