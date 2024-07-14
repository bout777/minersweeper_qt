
#include "game.h"

game::game(int rows_, int columns_, int num_mines_,QMainWindow *parent)
    : QMainWindow (parent)
{
    //初始化各参数
    game_time = new QTimer(this);
    this->rows = rows_;
    this->columns = columns_;
    this->num_mines = num_mines_;
    this->num_covered = rows_*columns_;
    buttons.resize(rows);
    IsFirclicked = false;
    //导入图标文件
    icons.push_back(QIcon(":/res/image/00.png"));
    icons.push_back(QIcon(":/res/image/01.png"));
    icons.push_back(QIcon(":/res/image/02.png"));
    icons.push_back(QIcon(":/res/image/03.png"));
    icons.push_back(QIcon(":/res/image/04.png"));
    icons.push_back(QIcon(":/res/image/05.png"));
    icons.push_back(QIcon(":/res/image/06.png"));
    icons.push_back(QIcon(":/res/image/07.png"));
    icons.push_back(QIcon(":/res/image/08.png"));
    icons.push_back(QIcon(":/res/image/09.png"));
    icons.push_back(QIcon(":/res/image/10.png"));
    icons.push_back(QIcon(":/res/image/11.png"));
    icons.push_back(QIcon(":/res/image/12.png"));
    icons.push_back(QIcon(":/res/image/mine_back.png"));
    icons.push_back(QIcon(":/res/image/mine_flag.png"));
    icons.push_back(QIcon(":/res/image/wrong_flag.png"));

    //创建独立线程实现音效播放
    this->audioThread = new QThread;

    QSoundEffect *begin = new QSoundEffect();
    begin->setSource(QUrl::fromLocalFile(":/res/music/begin.wav"));
    begin->moveToThread(audioThread);

    this->spread = new QSoundEffect();
    spread->setSource(QUrl::fromLocalFile(":/res/music/spread.wav"));
    connect(this,&game::play_spread,spread,&QSoundEffect::play);
    spread->moveToThread(audioThread);

    left_right = new QSoundEffect();
    left_right->setSource(QUrl::fromLocalFile(":/res/music/left_right.wav"));
    connect(this,&game::play_left_right,left_right,&QSoundEffect::play);
    left_right->moveToThread(audioThread);

    audioThread->start();
    begin->play();

    //初始化雷区按钮
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < columns; ++col) {

            mybutton *button = new mybutton(this);
            buttons[row].push_back(button); //将按钮对象放进指针数组中，便于后续对按钮的操作

            //按钮的初始设置
            button->setFixedSize(50,50);
            button->installEventFilter(this);
            button->setIcon(icons[9]);
            button->setIconSize(button->size());

            connect(button,&mybutton::leftButtonClicked,this,[=](){
                if (board[row][col] == SquareState::Covered){
                    button->setIcon(icons[0]);
                }
            });

            connect(button, &mybutton::leftButtonReleased, this, [=]() {
                if(!IsFirclicked){
                    set_mine_aera(row,col);
                    IsFirclicked = true;
                    game_time->start(1000);
                }
                this->handleLeftClick(row,col);
            });

            connect(button, &mybutton::entered,this,[=](){
                if(board[row][col] == SquareState::Covered){
                    button->setIcon(icons[10]);
                }else if(board[row][col]==SquareState::Flagged){
                    button->setIcon(icons[12]);
                }
            });

            connect(button, &mybutton::left,this,[=](){
                if(board[row][col] == SquareState::Covered){
                    button->setIcon(icons[9]);
                }else if(board[row][col]==SquareState::Flagged){
                    button->setIcon(icons[11]);
                }
            });

            connect(button, &mybutton::rightButtonClicked,this,[=](){
                if(board[row][col] == SquareState::Covered){
                    button->setIcon(icons[11]);
                    board[row][col] = SquareState::Flagged;
                    emit flag_set();
                }else if(board[row][col]== SquareState::Flagged){
                    button->setIcon(icons[9]);
                    board[row][col] = SquareState::Covered;
                    emit FlagDecrase();
                }
            });

            connect(button,&mybutton::rightAndleftClicked,this,[=](){
                if(board[row][col] == SquareState::Uncovered){
                    int flag_num = 0;
                    for (int i = -1;i<=1;++i){  //遍历周围方块,统计棋子数量
                        for (int j = -1; j <=1; ++j) {
                            int x = row + i;
                            int y = col + j;
                            if(x<0||x>=rows||y<0||y>=columns){  //是否越界
                                continue;
                            } else if(board[x][y] == SquareState::Flagged){
                                ++flag_num;
                            }
                        }
                    }
                    if(flag_num!=mine_num[row][col]){
                        emit play_left_right();
                        for (int i = -1;i<=1;++i) {  //遍历周围方块,显示动画效果
                            for (int j = -1; j <=1; ++j) {
                                int x = row + i;
                                int y = col + j;
                                if(x<0||x>=rows||y<0||y>=columns) {  //是否越界
                                    continue;
                                }else if(board[x][y] == SquareState::Flagged||board[x][y] == SquareState::Uncovered) {
                                    continue;
                                }else if(i!=0||j!=0) {
                                    buttons[x][y]->setIcon(icons[0]);
                                }
                            }
                        }
                    } else{
                        for (int i = -1;i<=1;++i){      //遍历周围方块,自动点击未标志区域
                            for (int j = -1; j <=1; ++j) {
                                int x = row + i;
                                int y = col + j;
                                if(x<0||x>=rows||y<0||y>=columns) {  //是否越界
                                    continue;
                                }if(board[x][y]==SquareState::Covered){
                                    handleLeftClick(x,y);
                                }
                            }
                        }
                    }
                }
            });

            connect(button,&mybutton::rightOrleftRelease,this,[=](){
                if(board[row][col]==SquareState::Uncovered){
                    for (int i = -1;i<=1;++i){  //遍历周围方块,统计棋子数量
                        for (int j = -1; j <=1; ++j) {
                            int x = row + i;
                            int y = col + j;
                            if(x<0||x>=rows||y<0||y>=columns){  //是否越界
                                continue;
                            }else if(board[x][y]==SquareState::Covered){
                                if(i!=0||j!=0)
                                    buttons[x][y]->setIcon(icons[9]);
                            }
                        }
                    }
                }
            });

        }
    }

    //处理游戏胜利逻辑
    connect(this,&game::win,this,[=](){
        qDebug("win");
        for(int i = 0;i<rows;i++){
            for(int j = 0;j<columns;j++){
                if(ifmine[i][j]){
                    uncover(i,j);
                    buttons[i][j]->setIcon(icons[14]);
                }
            }
        }
        emit win_2();
    });
}

game::~game(){
}

void game::handleLeftClick(int row, int col) {
    QQueue<block*> *queue = new QQueue<block*>;
    if(board[row][col]==SquareState::Covered){
        if(ifmine[row][col] == false){
            if(mine_num[row][col]==0){//利用BFS自动翻开相邻联通块
                emit play_spread();
                block *now = new block;
                now->x = row;
                now->y = col;
                queue->enqueue(now);
                uncover(row,col);
                QTimer *timer = new QTimer;
                timer->setInterval(3);
                connect(timer,&QTimer::timeout,this,[=](){
                    block * cur = queue->dequeue();
                    for (int i = -1;i<=1;++i){  //遍历周围方块
                        for (int j = -1; j <=1; ++j) {
                            int x = cur->x + i;
                            int y = cur->y + j;
                            if(x<0||x>=rows||y<0||y>=columns){  //是否越界
                                continue;
                            }else if(board[x][y]==SquareState::Uncovered||board[x][y]==SquareState::Flagged){//是否已访问
                                continue;
                            }else if(mine_num[x][y]!=0){    //是否雷数不为0
                                uncover(x,y);
                            }else {
                                block *now = new block;
                                now->x = x;
                                now->y = y;
                                queue->enqueue(now);
                                uncover(x,y);
                            }
                        }
                    }
                    if(queue->isEmpty()){
                        timer->stop();
                    }
                });
                timer->start();
            }else{
                uncover(row,col);
            }

        }else {
            //处理游戏失败逻辑
            for(int i = 0;i<rows;i++){
                for(int j = 0;j<columns;j++){
                    if(board[i][j]==SquareState::Flagged||ifmine[i][j])
                        uncover(i,j);
                }
            }
            emit lost();
        }
    }
}

void game::uncover(int x ,int y){
    if(board[x][y]==SquareState::Covered){
        if(ifmine[x][y]==false){
            board[x][y]=SquareState::Uncovered;
            buttons[x][y]->setIcon(icons[mine_num[x][y]]);
            this->num_covered--;
            if(num_covered==num_mines){
                qDebug("runin");
                emit win();
            }
        }else{
            board[x][y]=SquareState::Uncovered;
            buttons[x][y]->setIcon(icons[13]);
        }
    }else if(board[x][y]==SquareState::Flagged){
        if(ifmine[x][y]==false){
            board[x][y]=SquareState::Uncovered;
            buttons[x][y]->setIcon(icons[15]);
        }else{
            board[x][y]=SquareState::Uncovered;
            buttons[x][y]->setIcon(icons[14]);
        }
    }
}

void game::set_mine_aera(int first_x,int first_y){
    ifmine.resize(rows);
    mine_num.resize(rows);
    for (int row = 0; row <rows; ++row) {
        ifmine[row].resize(columns);
        mine_num[row].resize(columns);
        for (int col = 0; col < columns; ++col) {
            ifmine[row][col] = false;
            mine_num[row][col] = 0;
        }
    }
    srand(time(0));
    int count = 0;

    //随机生成雷区
    while(count<num_mines){
        int x = rand()%rows;
        int y = rand()%columns;
        if(ifmine[x][y]==false&&[=](){ //利用lambda表达式，处理首次点击区域不生成雷的逻辑
                for (int i = -1; i <= 1; ++i){
                    for(int j = -1 ; j <= 1;++j){
                        if(x==first_x+i&&y==first_y+j){
                            return false;
                        }
                    }
                }
                return true;
            }()){
            ++count;
            ifmine[x][y]=true;
        }
    }
    for (int row = 0; row <rows; ++row) {
        for (int col = 0; col < columns; ++col) {
            if(ifmine[row][col])
                continue;
            int count = 0;
            for(int dx = -1;dx<=1;++dx){
                for(int dy = -1;dy<=1;++dy) {
                    int x = row + dx;
                    int y = col + dy;
                    if(x>=0&&x<rows&&y>=0&&y<columns&&ifmine[x][y])
                        ++count;
                }
            }
            mine_num[row][col] = count;
        }
    }
}

void game::set_board() {
    //初始化方块状态
    board.resize(rows);
    for(int row = 0;row<rows;++row){
        board[row].resize(columns);
        for(int col = 0;col < columns;++col){
            board[row][col] = SquareState::Covered;
        }
    }
}

QVector<QVector<mybutton*>>& game::get_buttons(){
    return this->buttons;
}

QTimer* game::get_time(){
    return this->game_time;
}

void game::setButton(){
    for(int i = 0;i < this->rows;++i){
        for (int j = 0;j < this->columns;++j){
            if(this->board[i][j]==game::SquareState::Covered){
                this->buttons[i][j]->setIcon(icons[9]);
            }else if(this->board[i][j]==game::SquareState::Flagged){
                this->buttons[i][j]->setIcon(icons[11]);
            }else if(this->board[i][j]==game::SquareState::Uncovered){
                buttons[i][j]->setIcon(icons[mine_num[i][j]]);
            }
        }
    }
}

QDataStream& operator<<(QDataStream& out,const game& g){
    //输出游戏数据
    out << g.rows;
    out << g.columns;
    out << g.num_mines;
    out << g.num_covered;
    for(int i = 0;i < g.rows;++i){
        for (int j = 0;j<g.columns;++j){
            out << g.ifmine[i][j];
            out << g.board[i][j];
            out << g.mine_num[i][j];
        }
    }
    return out;
}

void game::setData(QDataStream &in){
    //导入游戏数据
    this->IsFirclicked = true;
    board.clear();
    ifmine.clear();
    mine_num.clear();
    board.resize(rows);
    ifmine.resize(rows);
    mine_num.resize(rows);
    for(int i = 0;i < this->rows;++i){
        board[i].resize(columns);
        ifmine[i].resize(columns);
        mine_num[i].resize(columns);
        for (int j = 0;j < this->columns;++j){
            qDebug()<<this->ifmine.size();
            in >> this->ifmine[i][j];
            in >> this->board[i][j];
            in >> this->mine_num[i][j];
        }
    }
}
