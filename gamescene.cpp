#include "gamescene.h"
#include <QDebug>
#include <QMessageBox>
#include <QTextStream>


GameScene::GameScene(QWidget *parent, int _type)
    :QGraphicsScene(parent), n(14), pressed(-1), type(_type),  disable(true)
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
    ss<<Head<<endl<<type<<endl;
    for(int i=0;i<2;i++){
        ss<<history[i].size()<<" "<<undo[i]<<endl;
        for(int j=0;j<history[i].size();j++)
            ss<<history[i][j].x()<<" "<<history[i][j].y()<<" ";
        ss<<endl;
    }
    qDebug()<<tmp;
    return tmp;
}

bool GameScene::fromString(QString str)
{
    const QString Head = "Gomoku Save Data";
    QTextStream ss(&str);
    QString tmp="";
    tmp = ss.readLine();
    if(tmp != Head)return false;
    ss >> type;
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
    if(disable){
        QColor col = QColor(Qt::gray);
        col.setAlpha(128);
        this->addRect(0, 0, size, size, QPen(col), QBrush(col));
    }
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

bool GameScene::Undo()
{
    if(!history[0].size() || !history[1].size())return false;
    for(int i=0;i<2;i++)
    {
        map[history[i].back().x()][history[i].back().y()]=-1;
        history[i].removeLast();
    }
    drawMap();
    return (++undo[type] >= 2);
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
                    memset(undo, 0, sizeof(undo));
                    history[map[i][j]].push_back(QPoint(i, j));
                    emit exchange(type);
                }
                break;
            }
        }
    drawMap();
    check();
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
                    QMessageBox::information(0, QString("Game End"), QString("Player%1 Win!").arg(map[i][j]+1));
                    Init();
                    return map[i][j];
                }
            }

       }

}
