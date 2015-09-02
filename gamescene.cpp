#include "gamescene.h"
#include <QDebug>
#include <QMessageBox>
#include <QTextStream>


GameScene::GameScene(QWidget *parent, int _type)
    :QGraphicsScene(parent), n(14), pressed(-1), type(_type), disable(true), move(false)
{
    //this->setBackgroundBrush(Qt::yellow);
    Init();
}

void GameScene::Init()
{
    disable = true;
    memset(map, -1, sizeof(map));
    history[0].clear();
    history[1].clear();
    drawMap();
}

QString GameScene::toString()
{
    QString tmp="";
    const QString Head = "Gomoku Save Data";
    QTextStream ss(&tmp);
    ss<<Head<<endl;
    for(int i=0;i<2;i++){
        ss<<history[i].size()<<" "<<undo[i]<<endl;
        for(int j=0;j<history[i].size();j++)
            ss<<history[i][j].x()<<" "<<history[i][j].y()<<" ";
        ss<<endl;
    }
    qDebug()<<tmp;
    return tmp;
}

void GameScene::setChess(int x, int y)
{
    map[x][y]=(type^1);
    history[type^1].push_back(QPoint(x, y));
    move^=1;
    drawMap();
}

bool GameScene::fromString(QString str)
{
    const QString Head = "Gomoku Save Data";
    QTextStream ss(&str);
    QString tmp="";
    tmp = ss.readLine();
    if(tmp != Head)return false;
    memset(map, -1, sizeof(map));
    for(int i=0;i<2;i++){
        history[i].clear();
        int n;ss>>n>>undo[i];
        for(int j=0;j<n;j++)
        {
            int x, y;ss>>x>>y;
            map[x][y]=i;
            history[i].push_back(QPoint(x, y));
        }
    }
    disable = true;
    pressed = -1;
    bool current = (history[0].size()>history[1].size());
    move = (current == type);
    drawMap();
    return true;
}

void GameScene::drawMap(int x, int y)
{
    this->clear();
    const double del = Psize/2;
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
        {
            QPen pen = QPen(Qt::gray);
            QBrush brush = QBrush(Qt::white);
            this->addRect(i*Psize+del, j*Psize+del, Psize, Psize, pen, brush);
        }
    for(int i=0;i<=n;i++)
        for(int j=0;j<=n;j++){
            QColor col = QColor((map[i][j]?(Qt::black):(Qt::blue)));
            circle[i][j]=this->addEllipse(i*Psize+del-del/2, j*Psize+del-del/2, Psize/2, Psize/2, QPen(col), QBrush(col));
            if(map[i][j]==-1)circle[i][j]->setVisible(false);
        }
    if(pressed <= 1 && pressed >=0){
        QColor col = QColor((pressed?(Qt::black):(Qt::blue)));
        this->addEllipse(x-del/2, y-del/2, Psize/2, Psize/2, QPen(col), QBrush(col));
    }
    if(disable)
    {
        QColor col = QColor(Qt::gray);
        col.setAlpha(128);
        this->addRect(0, 0, size, size, QPen(col), QBrush(col));
    }
    else
        check();

}

void GameScene::Resize(int _size)
{
    size = _size;
    Psize = double(size)/double(n+1);
    drawMap();
}

void GameScene::Press(int x, int y, int _type)
{
    //qDebug()<<x<<" "<<y<<" "<<_type;
    pressed = type;
    drawMap(x, y);
}


void GameScene::Move(int x, int y)
{
    //qDebug()<<x<<" "<<y<<" "<<type;
    drawMap(x, y);
}

bool GameScene::Undo(int x)
{
    int current = (history[0].size()>history[1].size());
    if(history[x].size())
    {
        map[history[x].back().x()][history[x].back().y()]=-1;
        history[x].removeLast();
        if(current == x){
            map[history[x^1].back().x()][history[x^1].back().y()]=-1;
            history[x^1].removeLast();
        }
        move = (type == x);
    }else return false;
    drawMap();
    ++undo[x];
    return (undo[type] >= 2);
}

void GameScene::Release(int x, int y, int _type)
{
    pressed=-1;
    for(int i=0;i<=n;i++)
        for(int j=0;j<=n;j++)
        {
            int fx=x-circle[i][j]->rect().x(),  fy=y-circle[i][j]->rect().y();
            if(fx>0 && fy>0 && fx<Psize/2 && fy<Psize/2)
            {
                if(map[i][j] == -1){
                    map[i][j] = type;//, type^=1;
                    move^=1;
                    memset(undo, 0, sizeof(undo));
                    history[map[i][j]].push_back(QPoint(i, j));
                    emit moveChess(i, j);
                }
                break;
            }
        }
    drawMap();
}

int GameScene::check()
{
    for(int i=0;i<=n;i++)
       for(int j=0;j<=n;j++)if(map[i][j]!=-1)
       {
            const int dx[4]={1, 0, 1, -1},dy[4]={0, 1, 1, 1};
            for(int k=0;k<4;k++)
            {
                int num=0;
                for(int fx=i, fy=j;fx>=0 && fy>=0 && fx<=n && fy<=n && map[fx][fy]==map[i][j];fx+=dx[k], fy+=dy[k])num++;
                if(num >= 5)
                {
                    emit gameEnd(map[i][j]);
                    return map[i][j];
                }
            }

       }

}
