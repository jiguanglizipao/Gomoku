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
#include "gamesolver.h"

class GameScene : public QGraphicsScene
{
    Q_OBJECT

public:
    GameScene(QWidget *parent = 0, int _size = 700);
    void Init(Data &a);
    void Solve(const QVector<QVector<QPoint> > &_path);
    void Resize(int _size);
    void Press(int x, int y);
    void Move(int x, int y);
    void Release(int x, int y);
private:
    int size, n;
    double Psize, FlagR, CircleR, PathR;
    int moveFlag;
    void makeMap();
    void drawCircle(int i, int j, QPen pen = QPen(), QBrush brush = QBrush());
    QVector<Flags> flags;
    void addFlag(Flags flag);
    QVector<QVector<int> > filled;
    QVector<QVector<QPoint> > path;
    bool checkFlag(int x, int y);
    void Update(int x=0, int y=0);
    QSound *Broke, *Flow, *Success;
    /*double distance(QPoint x)
    {
        return qSqrt(qPow(x.x(), 2) + qPow(x.y(), 2));
    }*/
};

#endif // GAMESCENE_H
