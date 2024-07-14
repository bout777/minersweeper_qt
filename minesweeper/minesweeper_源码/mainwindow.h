
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QVector>
#include "game.h"
#include "gamerecord.h"
#include <QDialog>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QThread>
#include <QSettings>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT
signals:
    void mouse_move();
    void resizing();
    void close_1();
    void close_2();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void set_game_aera(int rows,int columns);
    int handle_first_click();
    game* RunGame(int,int,int);
    QPoint now_point;
    void set_layout();
    void game_restar();
    void SaveGame();
    void LoadGame();
private:
    Ui::MainWindow *ui;
    game *newGame;
    QGridLayout *lay;
    QTimer *GameTime;
    GameRecord *record;
    GameMode Now_mode;
    int *time_display;
    int *mine_display;
protected:
    bool eventFilter(QObject* watched, QEvent* event) override{
        if (event->type() == QEvent::Close) {
            connect(this,&MainWindow::close_2,this,[=](){
                event->accept();
            });
        }
        return QMainWindow::eventFilter(watched, event);
    }

    void mouseMoveEvent(QMouseEvent * event) override{
        emit mouse_move();
        this->now_point = event->pos();
        QMainWindow::mouseMoveEvent(event);
    }

    void closeEvent(QCloseEvent *ev)override{
        ev->ignore();
        emit close_1();
    }


    void resizeEvent(QResizeEvent *ev)override{
        emit resizing();
        QMainWindow::resizeEvent(ev);
    }
};

#endif // MAINWINDOW_H
