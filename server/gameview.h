#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QObject>
#include <QWidget>
#include <QResizeEvent>
#include "gamescene.h"

class GameView : public QGraphicsView
{
    Q_OBJECT
public:
    GameView(QWidget *parent = 0);
    void setScene(GameScene *scene);
protected:
    void mousePressEvent(QMouseEvent  *event);
    void mouseMoveEvent(QMouseEvent  *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent * event);
signals:
    void Press(int x, int y);
    void Move(int x, int y);
    void Release(int x, int y);
private:
    GameScene *Scene;
};

#endif // GAMEVIEW_H
