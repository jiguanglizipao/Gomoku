#ifndef GAMESOLVER_H
#define GAMESOLVER_H
#include <QPoint>
#include <QBrush>
#include <QPen>
#include <QVector>


struct Flags
{
    QPoint a[2];
    QBrush brush;
    QPen pen;
    Flags(QPoint a1=QPoint(), QPoint a2=QPoint(), QPen _pen = QPen(), QBrush _brush = QBrush())
    {
        a[0]=a1, a[1]=a2, brush=_brush, pen=_pen;
    }
};

struct Data
{
    QVector<Flags> flags;
    int n;
};

class GameSolver
{
public:
    QVector<QVector<QPoint> > operator()(const Data &data);
private:
    QVector<QVector<int> > f;
    QVector<QVector<QPoint> > path;
    bool dfs(int deep, int x, int y, const Data &data);
};

#endif // GAMESOLVER_H
