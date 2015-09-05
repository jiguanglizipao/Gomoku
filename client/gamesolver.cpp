#include "gamesolver.h"


QVector<QVector<QPoint> > GameSolver::operator()(const Data &data)
{
    f.clear();
    f.resize(data.n);
    for(int i=0;i<data.n;i++)
    {
        f[i].resize(data.n);
        for(int j=0;j<data.n;j++)f[i][j]=-1;
    }
    for(int i=0;i<data.flags.size();i++)
    {
        f[data.flags[i].a[0].x()][data.flags[i].a[0].y()]=i;
        f[data.flags[i].a[1].x()][data.flags[i].a[1].y()]=i;
    }
    path.clear();path.resize(data.flags.size());
    path[0].clear();path[0].push_back(QPoint(data.flags[0].a[0].x(), data.flags[0].a[0].y()));
    if(dfs(0, data.flags[0].a[0].x(), data.flags[0].a[0].y(), data))return path;else return QVector<QVector<QPoint> >();
}

bool GameSolver::dfs(int deep, int x, int y, const Data &data)
{
    if(x==data.flags[deep].a[1].x() && y==data.flags[deep].a[1].y())
    {
        if(deep == data.flags.size()-1)
        {
            for(int i=0;i<data.n;i++)
                for(int j=0;j<data.n;j++)
                    if(f[i][j] == -1)return false;
            return true;
        }
        else
        {
            path[deep+1].clear();path[deep+1].push_back(QPoint(data.flags[deep+1].a[0].x(), data.flags[deep+1].a[0].y()));
            return dfs(deep+1, data.flags[deep+1].a[0].x(), data.flags[deep+1].a[0].y(), data);
        }
    }

    int px=(data.flags[deep].a[1].x()-x)?((data.flags[deep].a[1].x()-x)/abs(data.flags[deep].a[1].x()-x)):1;
    int py=(data.flags[deep].a[1].y()-y)?((data.flags[deep].a[1].y()-y)/abs(data.flags[deep].a[1].y()-y)):1;
    const int dx[4]={px, 0, -px, 0};
    const int dy[4]={0, py, 0, -py};

    for(int i=0;i<4;i++)
    {
        int fx=x+dx[i], fy=y+dy[i], size=path[deep].size();
        if(fx<0 || fy<0 || fx>=data.n || fy>=data.n || (f[fx][fy]!=-1&&(fx!=data.flags[deep].a[1].x()||fy!=data.flags[deep].a[1].y())))continue;
        path[deep].push_back(QPoint(fx, fy));
        f[fx][fy] = deep;
        if(dfs(deep, fx, fy, data))return true;
        while(path[deep].size()>size)
            path[deep].removeLast();
        if(fx!=data.flags[deep].a[1].x()||fy!=data.flags[deep].a[1].y())f[fx][fy] = -1;
    }
    return false;
}
