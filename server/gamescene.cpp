#include "gamescene.h"
#include <QDebug>

GameScene::GameScene(QWidget *parent, int _size)
    :QGraphicsScene(parent), size(_size), n(0), moveFlag(-1)
{
    this->setBackgroundBrush(Qt::black);
    Broke = new QSound("://res/broke.wav", this);
    Flow = new QSound("://res/flow.wav", this);
    Success = new QSound("://res/success.wav", this);
}

void GameScene::Init(Data &a)
{
    n=a.n;
    Psize = double(size)/double(n);
    filled.resize(n);
    for(int i=0;i<n;i++)filled[i].resize(n);
    for(int i=0;i<n;i++)for(int j=0;j<n;j++)filled[i][j]=-1;
    FlagR = Psize*.6;
    CircleR = Psize*.8;
    PathR = Psize*.3;
    flags = a.flags;
    path.resize(flags.size());
    for(int i=0;i<path.size();i++)path[i].clear();
    Update(false);
}

void GameScene::Resize(int _size)
{
    size = _size;
    Psize = double(size)/double(n);
    FlagR = Psize*.6;
    CircleR = Psize*.8;
    PathR = Psize*.3;
    Update(false);
}

void GameScene::makeMap()
{
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
        {
            QPen pen = QPen(Qt::gray);
            QBrush brush = QBrush(Qt::black);
            this->addRect(i*Psize, j*Psize, Psize, Psize, pen, brush);
            if(filled[i][j]!=-1 && filled[i][j]!=moveFlag){
                brush = flags[filled[i][j]].brush;
                QColor tmp = flags[filled[i][j]].brush.color();
                tmp.setAlpha(128);
                brush.setColor(tmp);
                this->addRect(i*Psize, j*Psize, Psize, Psize, pen, brush);
            }

        }
    for(int i=0;i<flags.size();i++)addFlag(flags[i]);
}

void GameScene::addFlag(Flags flag)
{
    const double r = FlagR;
    this->addEllipse(flag.a[0].x()*Psize+Psize/2-r/2, flag.a[0].y()*Psize+Psize/2-r/2, r, r, flag.pen, flag.brush);
    this->addEllipse(flag.a[1].x()*Psize+Psize/2-r/2, flag.a[1].y()*Psize+Psize/2-r/2, r, r, flag.pen, flag.brush);
}

void GameScene::drawCircle(int x, int y, QPen pen, QBrush brush)
{
    const double r = CircleR;
    QColor tmp = brush.color();tmp.setAlpha(64);
    brush.setColor(tmp);
    this->addEllipse(x-r/2, y-r/2, r, r, pen, brush);
}

void GameScene::Press(int x, int y)
{
    moveFlag=-1;
    int tmp;
    for(int i=0;i<flags.size();i++)
        for(int j=0;j<2;j++)
        {
            double fx=flags[i].a[j].x(), fy=flags[i].a[j].y();
            if(x>=fx*Psize && x<(fx+1)*Psize && y>=fy*Psize && y<(fy+1)*Psize)moveFlag=i, tmp=j;
        }
    if(moveFlag!=-1)
    {
        for(int i=0;i<path[moveFlag].size();i++)filled[path[moveFlag][i].x()][path[moveFlag][i].y()]=-1;
        path[moveFlag].clear();
        path[moveFlag].push_back(flags[moveFlag].a[tmp]);
        filled[flags[moveFlag].a[tmp].x()][flags[moveFlag].a[tmp].y()]=moveFlag;
    }
    else
    {
        int fx, fy;
        for(int i=0;i<n;i++)
            for(int j=0;j<n;j++)
            {
                if(x>=i*Psize && x<(i+1)*Psize && y>=j*Psize && y<(j+1)*Psize)moveFlag=filled[i][j], fx=i, fy=j;
            }
        if(moveFlag!=-1)
        {
            while(path[moveFlag].back()!=QPoint(fx, fy))
                filled[path[moveFlag].back().x()][path[moveFlag].back().y()]=-1, path[moveFlag].removeLast();
        }
    }
    Update(x, y);
}

bool GameScene::checkFlag(int x, int y)
{
    for(int i=0;i<flags.size();i++)for(int j=0;j<2;j++)
        if(flags[i].a[j].x() == x && flags[i].a[j].y() == y)return (i==moveFlag);
    return true;
}

void GameScene::Move(int x, int y)
{
    x=std::max(0, x);x=std::min(size-1, x);
    y=std::max(0, y);y=std::min(size-1, y);

    const int dx[4]={-1, 0, 1, 0}, dy[4]={0, 1, 0, -1};

    if(moveFlag!=-1)
    {
        for(int i=0;i<4;i++)
        {
            int fx=path[moveFlag].back().x()+dx[i], fy=path[moveFlag].back().y()+dy[i];
            if(x>=fx*Psize && x<(fx+1)*Psize && y>=fy*Psize && y<(fy+1)*Psize){
                if(path[moveFlag].size()>1 && path[moveFlag][path[moveFlag].size()-2] == QPoint(fx, fy))
                    filled[path[moveFlag].back().x()][path[moveFlag].back().y()]=-1, path[moveFlag].removeLast();
                if(filled[fx][fy]!=moveFlag && checkFlag(fx, fy))
                {
                    if(path[moveFlag].size()>1 && (path[moveFlag].back()==flags[moveFlag].a[0]||path[moveFlag].back()==flags[moveFlag].a[1])) return Update(x, y);
                    if(filled[fx][fy] != -1)
                    {
                        Broke->play();
                        int tmp = filled[fx][fy];
                        for(;path[tmp].size();path[tmp].removeLast()){
                            filled[path[tmp].back().x()][path[tmp].back().y()]=-1;
                            if(path[tmp].back().x()==fx && path[tmp].back().y()==fy){
                                path[tmp].removeLast();
                                break;
                            }
                        }
                        //path[tmp].clear();
                    }
                    path[moveFlag].push_back(QPoint(fx, fy));
                    filled[fx][fy]=moveFlag;
                    if(path[moveFlag].size()>1 && (path[moveFlag].back()==flags[moveFlag].a[0]||path[moveFlag].back()==flags[moveFlag].a[1])) Flow->play();
                }
            }
        }
        /*
        for(size_t k=0;k<path[moveFlag].size();k++)
        {
            for(int i=0;i<4;i++)
            {
                int fx=path[moveFlag][k].x()+dx[i], fy=path[moveFlag][k].y()+dy[i];
                if(x>=fx*Psize && x<(fx+1)*Psize && y>=fy*Psize && y<(fy+1)*Psize){
                    while(path[moveFlag].size()>k+1)
                        filled[path[moveFlag].back().x()][path[moveFlag].back().y()]=-1, path[moveFlag].removeLast();
                    if(filled[fx][fy]!=moveFlag && checkFlag(fx, fy))
                    {
                        if(filled[fx][fy] != -1)
                        {
                            int tmp = filled[fx][fy];
                            for(size_t j=0;j<path[tmp].size();j++)filled[path[tmp][j].x()][path[tmp][j].y()]=-1;
                            path[tmp].clear();
                        }
                        path[moveFlag].push_back(QPoint(fx, fy));
                        filled[fx][fy]=moveFlag;
                    }
                }
            }
        }
        */
    }
    Update(x, y);
}

void GameScene::Release(int x, int y)
{
    if(moveFlag!=-1){
        bool f=true;
        for(int i=0;i<n;i++)for(int j=0;j<n;j++)if(filled[i][j] == -1)
        {
            f = false;
            break;
        }
        if(f)Success->play();
    }
    moveFlag = -1;
    Update(x, y);
}

void GameScene::Solve(const QVector<QVector<QPoint> > &_path)
{
    if(_path.empty()){
        qDebug()<<"no answer";
        return;
    }
    path = _path;
    for(int i=0;i<n;i++)for(int j=0;j<n;j++)filled[i][j]=-1;
    for(int i=0;i<path.size();i++)for(int j=0;j<path[i].size();j++)filled[path[i][j].x()][path[i][j].y()]=i;
    Success->play();
    Update();
}

void GameScene::Update(int x, int y)
{
    this->clear();
    if(!n)return;
    makeMap();
    /*
    for(size_t i=0;i<path.size();i++)
        for(int j=0;j<int(path[i].size())-1;j++)
        {
            int x = (path[i][j].x()==path[i][j+1].x())?(Psize*path[i][j].x()+Psize/2-PathR/2):(Psize*std::min(path[i][j].x(), path[i][j+1].x())+Psize/2);
            int y = (path[i][j].y()==path[i][j+1].y())?(Psize*path[i][j].y()+Psize/2-PathR/2):(Psize*std::min(path[i][j].y(), path[i][j+1].y())+Psize/2);
            int w = (path[i][j].x()==path[i][j+1].x())?PathR:Psize;
            int h = (path[i][j].y()==path[i][j+1].y())?PathR:Psize;
            this->addRect(x, y, w, h, flags[i].pen, flags[i].brush);
        }
    */
    for(int i=0;i<path.size();i++)
        for(int j=0;j<path[i].size()-1;j++)
        {
            QPen pen(flags[i].brush, PathR, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            this->addLine(Psize*path[i][j].x()+Psize/2, Psize*path[i][j].y()+Psize/2, Psize*path[i][j+1].x()+Psize/2, Psize*path[i][j+1].y()+Psize/2, pen);
        }
    if(moveFlag!=-1)drawCircle(x, y, flags[moveFlag].pen, flags[moveFlag].brush);
}
