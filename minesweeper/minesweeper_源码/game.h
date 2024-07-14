
#ifndef GAME_H
#define GAME_H


#include <QMainWindow>
#include <QObject>
#include <QGridLayout>
#include <QPushButton>
#include <QQueue>
#include <QIcon>
#include "mybutton.h"
#include <QTimer>
#include <QSoundEffect>
#include <QThread>

//extern bool isfirclick;


class game : public QMainWindow//创建游戏对象
{
    Q_OBJECT
public:
    game(int rows, int columns,int num_mines,QMainWindow *parent = nullptr);
    ~game();
    void handleLeftClick(int row, int column);
    void handleRightClick(int row, int column);
    void uncover(int row,int col);
    void set_mine_aera(int first_x,int first_y);
    void set_board();
    void setButton();
    void setData(QDataStream &in);
    bool isFirstClicked(){return IsFirclicked;}
    QTimer* get_time();
    QVector<QVector<mybutton*>>& get_buttons();
    int num_mines;
    int rows;
    int columns;
    int num_covered;
signals:
    void win();
    void win_2();
    void lost();
    void flag_set();
    void FlagDecrase();
    void play_spread();
    void play_left_right();

private:
    bool IsFirclicked;
    QSoundEffect *spread;
    QSoundEffect *left_right;
    QThread* audioThread;
    enum class SquareState {
        Covered,
        Uncovered,
        Flagged
    };
    QVector<QVector<bool>> ifmine;
    QVector<QVector<SquareState>> board;
    QVector<QVector<int>> mine_num;
    QVector<QVector<mybutton*>> buttons;
    QVector<QIcon> icons;
    struct block{
        int x;
        int y;
    };
    QTimer *game_time;
    friend QDataStream& operator<<(QDataStream& out,const game& g);

protected:
//        void mouseMoveEvent(QMouseEvent* event) override
//        {
//            // 获取鼠标的当前位置
//            QPoint mousePos = event->pos();
//            // 输出鼠标坐标
//            qDebug() << "Mouse position: " << mousePos;
//            // 进行其他处理
//            // ...
//        }
};

#endif // GAME_H
