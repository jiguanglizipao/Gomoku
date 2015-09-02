#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QObject>
#include <QWidget>
#include <QPoint>
#include <QtMath>
#include <QVector>
#include <QSound>
#include <QGraphicsEllipseItem>
#include "gamesolver.h"
#include <QVector>
#include <QPair>

class GameScene : public QGraphicsScene
{
    Q_OBJECT

public:
    GameScene(QWidget *parent = 0, int _type=0);
    QString toString();
    bool fromString(QString str);
    bool disable;
    bool move;
    int type;
public slots:
    void Init();
    bool Undo(int x);
    void Resize(int _size);
    void Press(int x, int y, int pressed);
    void Move(int x, int y);
    void Release(int x, int y, int pressed);
    void drawMap(int x=0, int y=0);
    void setChess(int x, int y);
signals:
    void moveChess(int x, int y);
    void gameEnd(int x);
private:
    int map[15][15];
    QGraphicsEllipseItem *circle[15][15];
    QVector<QPoint> history[2];
    int size, pressed, undo[2];
    const int n;
    double Psize;
    int check();
};

#endif // GAMESCENE_H
