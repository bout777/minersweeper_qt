
#include "mainwindow.h"
#include "ui_mainwindow.h"


QFile file("record.dat");
QFile gamedata("game.dat");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)//
    , ui(new Ui::MainWindow)
{
    installEventFilter(this);
    ui->setupUi(this);
    ui->centralwidget->setStyleSheet("background-color: #D2D5E2;");
    ui->centralwidget->setFixedSize(this->size().width(),this->size().height());
    ui->lcdNumber->setStyleSheet("background-color: #31579B;border-radius: 5px;");
    ui->lcdNumber_2->setStyleSheet("background-color: #31579B;border-radius: 5px;");

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(5);  // 设置阴影的模糊半径
    shadowEffect->setColor(Qt::black);  // 设置阴影的颜色
    shadowEffect->setOffset(-2, -2);  // 设置阴影的偏移量

    QGraphicsDropShadowEffect *shadowEffect_2 = new QGraphicsDropShadowEffect();
    shadowEffect_2->setBlurRadius(5);  // 设置阴影的模糊半径
    shadowEffect_2->setColor(Qt::black);  // 设置阴影的颜色
    shadowEffect_2->setOffset(-2, -2);  // 设置阴影的偏移量

    ui->lcdNumber->setGraphicsEffect(shadowEffect);
    ui->lcdNumber_2->setGraphicsEffect(shadowEffect_2);

    QLabel *timer_iconlabel = new QLabel();
    timer_iconlabel->setPixmap(QPixmap(":/res/image/timer.png"));
    timer_iconlabel->setFixedSize(50,50);

    QLabel *mine_iconlabel = new QLabel();
    mine_iconlabel->setPixmap(QPixmap(":/res/image/mine_.png"));
    mine_iconlabel->setFixedSize(50,50);

    ui->timer_layout->addWidget(timer_iconlabel,0,0);
    ui->timer_layout->addWidget(ui->lcdNumber,0,1);

    ui->mine_layout->addWidget(mine_iconlabel,0,0);
    ui->mine_layout->addWidget(ui->lcdNumber_2,0,1);

    QLabel *CounterIconLabel = new QLabel();
    CounterIconLabel->setPixmap(QPixmap(":/res/image/mine_.png"));
    CounterIconLabel->setFixedSize(50,50);
    this->newGame = nullptr;
    this->lay = nullptr;
    this->GameTime = nullptr;
    this->setWindowTitle("Minesweeper");
    this->setWindowIcon(QIcon(":/res/image/mine.png"));
    this->record = new GameRecord(this);
    // 创建按钮
    QPushButton restar("关闭");

    //设置界面内各按钮
    connect(ui->actionfirst,&QAction::triggered,this,[=]() {
        RunGame(10,10,10);
        this->setMinimumSize(700,700);
        emit resizing();
    });

    connect(ui->actionsecond,&QAction::triggered,this,[=]() {
        RunGame(16,16,40);
        this->setMinimumSize(1000,1000);
        emit resizing();
    });

    connect(ui->actionthrid,&QAction::triggered,this,[=]() {
        RunGame(16,30,99);
        this->setMinimumSize(1700,980);
        emit resizing();
    });

    connect(ui->actionrecord,&QAction::triggered,this,[=](){
        record->setDefault();
        record->exec();
    });

    //处理关闭逻辑
    connect(this,&MainWindow::close_1,this,[=](){
        SaveGame();
    });

    connect(this,&MainWindow::close_2,this,[=](){
        record->SaveFile(&file);
    });

    //统计信息重置
    connect(this->record,&GameRecord::reset,this,[=](){
        record->Reset();
        record->ShowRecord({0,0,0,0});
    });

    connect(this,&MainWindow::resizing,this,[=](){
        set_layout();
    });

    //载入文件
    this->record->LoadFile(&file);
    LoadGame();
}

MainWindow::~MainWindow()
{
    delete ui;
}

game* MainWindow::RunGame(int rows,int columns,int mines_num){
    //删除上次游戏
    if(newGame!=nullptr){
        for(int i = 0;i<newGame->get_buttons().size();i++){
            for(int j = 0;j<newGame->get_buttons().at(0).size();j++){
                newGame->get_buttons().at(i).at(j)->move(-100,-100);
            }
        }
        QLayoutItem *item;
        while((item = lay->takeAt(0)) != nullptr){
            if(QWidget *widget = item->widget()){
                lay->removeWidget(widget);
            }
            delete item;
        }
        delete lay;
        lay = nullptr;
        delete newGame;
        newGame = nullptr;
    }

    //生成新游戏
    newGame = new game(rows,columns,mines_num);
    lay = new QGridLayout();
    GameTime = newGame->get_time();
    newGame->set_board();

    if(columns==10){
        Now_mode = GameMode::Easy;
        this->setMinimumSize(700,700);
        emit resizing();
    }else if(columns==16){
        Now_mode = GameMode::Middle;
        this->setMinimumSize(1000,1000);
        emit resizing();
    }else if(columns==30){
        Now_mode = GameMode::Hard;
        this->setMinimumSize(1700,980);
        emit resizing();
    }

    this->time_display = new int;
    *time_display = 0;
    ui->lcdNumber->display(*time_display);

    connect(GameTime,&QTimer::timeout,this,[=](){
        (*time_display)++;
        ui->lcdNumber->display(*time_display);
    });

    this->mine_display = new int;
    *mine_display = mines_num;
    ui->lcdNumber_2->display(*mine_display);

    connect(newGame,&game::flag_set,this,[=](){
        (*mine_display)--;
        ui->lcdNumber_2->display(*mine_display);
    });

    connect(newGame,&game::FlagDecrase,this,[=](){
        (*mine_display)++;
        ui->lcdNumber_2->display(*mine_display);
    });

    for(int i = 0;i<rows;i++){
        for(int j = 0;j<columns;j++){
            lay->addWidget(newGame->get_buttons().at(i).at(j),i,j);
        }
    }

    //设置界面
    ui->frame->setLayout(lay);
    ui->frame->layout()->setSizeConstraint(QLayout::SetMinimumSize);
    ui->frame->layout()->setSpacing(1);
    ui->frame->layout()->setContentsMargins(0, 0, 0, 0);
    ui->frame->setStyleSheet("background-color: black;");
    ui->frame->setFixedSize(51*columns+2,51*rows+2);

    //统计对局信息
    connect(this->newGame,&game::win_2,this,[=](){
        record->UpdateWinNum(Now_mode);
        record->UpdateTime(ui->lcdNumber->intValue(),Now_mode);
    });

    connect(this->newGame,&game::lost,this,[=](){
        record->UpdateLostNum(Now_mode);
    });

    //进入新游戏处理
    game_restar();

    return newGame;
}

void MainWindow::set_layout(){
    ui->centralwidget->setFixedSize(this->size().width(),this->size().height());
    int x = (ui->centralwidget->size().width()-ui->frame->size().width())/2;
    int y = (ui->centralwidget->size().height()-ui->frame->size().height())/2;
    ui->frame->move(x,y);
}

void MainWindow::game_restar(){

    //创建新游戏对话框
    QDialog *restar = new QDialog(this);
    restar->setFixedSize(250,100);

    Qt::WindowFlags flags = restar->windowFlags();
    flags &= ~Qt::WindowContextHelpButtonHint;
    restar->setWindowFlags(flags);
    QLabel *dialogLabel = new QLabel(restar);

    QPushButton *button_restar = new QPushButton("再来一局",restar);
    connect(button_restar,&QPushButton::clicked,this,[=](){
        restar->accept();
    });


    QGridLayout *layout_1 = new QGridLayout(restar);
    layout_1->addWidget(dialogLabel,0,1,1,3);
    layout_1->addWidget(button_restar,1,1,1,2);

    connect(this->newGame,&game::win_2,this,[=](){
        restar->setWindowTitle("游戏胜利");
        QString str = QString("你赢了，用时%1s").arg(ui->lcdNumber->intValue());
        dialogLabel->setText(str);
        ui->lcdNumber_2->display(0);
        GameTime->stop();
        restar->open();
    });

    connect(this->newGame,&game::lost,this,[=](){
        restar->setWindowTitle("游戏失败");
        dialogLabel->setText("你输了，下次走运！");
        GameTime->stop();
        restar->open();
    });



    connect(restar,&QDialog::finished,this,[=](){
        if(Now_mode==GameMode::Easy){
            RunGame(10,10,10);
        }else if(Now_mode==GameMode::Middle){
            RunGame(16,16,40);
        }else if(Now_mode==GameMode::Hard){
            RunGame(16,30,99);
        }
    });
}

void MainWindow::SaveGame(){
    QDialog *close = new QDialog(this);
    close->setFixedSize(250,100);

    Qt::WindowFlags flags = close->windowFlags();
    flags &= ~Qt::WindowContextHelpButtonHint;
    close->setWindowFlags(flags);

    QPushButton *save = new QPushButton("保存此局",close);
    connect(save,&QPushButton::clicked,this,[=](){
        if(gamedata.open(QIODevice::WriteOnly)){
            qDebug("dataopen");
            QDataStream out(&gamedata);

            bool ifsave = true;
            out<<ifsave;

            out<<(*newGame);
            out<<(int)(*time_display);
            out<<(int)(*mine_display);

            gamedata.close();
        }
        close->accept();
        emit close_2();
    });

    QPushButton *exit = new QPushButton("直接退出",close);
    connect(exit,&QPushButton::clicked,this,[=](){
        if(gamedata.open(QIODevice::WriteOnly)){
            QDataStream out(&gamedata);
            bool ifsave = false;
            out << ifsave;
            out << newGame->rows;
            out << newGame->columns;
            out << newGame->num_mines;

            gamedata.close();
        }
        close->accept();
        emit close_2();
    });

    QGridLayout *layout = new QGridLayout(close);
    layout->addWidget(save,0,1);
    layout->addWidget(exit,0,2);
    if(newGame->isFirstClicked()){
        GameTime->stop();
        close->exec();
    }else {
        emit exit->clicked();
    }

}

void MainWindow::LoadGame(){

    if(gamedata.open(QIODevice::ReadOnly)){
        QDataStream in(&gamedata);
        bool ifsave;
        in>>ifsave;
        if(ifsave==true){
            int rows;
            int columns;
            int num_mines;
            int num_uncover;
            in >> rows;
            in >> columns;
            in >> num_mines;
            in >> num_uncover;

            RunGame(rows,columns,num_mines);

            newGame->setData(in);
            in >> *this->time_display;
            in >> *this->mine_display;
            ui->lcdNumber_2->display(*this->mine_display);

            newGame->setButton();
            ui->lcdNumber->display(*this->time_display);
            newGame->get_time()->start(1000);
        } else {
            int rows;
            int columns;
            int num_mines;
            in >> rows;
            in >> columns;
            in >> num_mines;

            RunGame(rows,columns,num_mines);
        }
        gamedata.resize(0);
        gamedata.close();
    }
}


