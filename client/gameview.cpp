#include "gameview.h"
#include <QDebug>
#include <QMouseEvent>
#include <QResizeEvent>
#include <iostream>

GameView::GameView(QWidget *parent)
    :QGraphicsView(parent)
{
}

void GameView::mousePressEvent(QMouseEvent  *event)
{
    if(Scene->disable || !Scene->move)return;
    QPointF tmp = mapToScene(event->pos());
    Scene->Press(tmp.x(), tmp.y(), event->button());
}

void GameView::mouseMoveEvent(QMouseEvent  *event)
{
    if(Scene->disable || !Scene->move)return;
    QPointF tmp = mapToScene(event->pos());
    Scene->Move(tmp.x(), tmp.y());
}

void GameView::mouseReleaseEvent(QMouseEvent  *event)
{
    if(Scene->disable || !Scene->move)return;
    QPointF tmp = mapToScene(event->pos());
    Scene->Release(tmp.x(), tmp.y(), event->button());
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


