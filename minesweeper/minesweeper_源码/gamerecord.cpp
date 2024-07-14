#include "gamerecord.h"
#include "ui_gamerecord.h"

GameRecord::GameRecord(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameRecord)
{
    //游戏统计信息，连接对应模式，展示信息
    ui->setupUi(this);

    connect(this->ui->easy,&QRadioButton::clicked,this,[=](){
        ShowRecord(this->Easy);
    });

    connect(this->ui->middle,&QRadioButton::clicked,this,[=](){
        ShowRecord(this->Middle);
    });

    connect(this->ui->hard,&QRadioButton::clicked,this,[=](){
        ShowRecord(this->Hard);
    });

    connect(this->ui->reset,&QPushButton::clicked,this,[=](){
        emit reset();
    });

    Qt::WindowFlags flags = this->windowFlags();
    flags &= ~Qt::WindowContextHelpButtonHint;
    this->setWindowFlags(flags);
    this->setWindowTitle("游戏统计");

}

GameRecord::~GameRecord()
{

    delete ui;
}

void GameRecord::LoadFile(QFile *file){
    //载入
    if(file->open(QIODevice::ReadOnly)){
        QDataStream in(file);
        in >> this->Easy.WinNum;
        in >> this->Easy.LostNum;
        in >> this->Easy.FastestTime;

        in >> this->Middle.WinNum;
        in >> this->Middle.LostNum;
        in >> this->Middle.FastestTime;

        in >> this->Hard.WinNum;
        in >> this->Hard.LostNum;
        in >> this->Hard.FastestTime;
        file->resize(0);
        file->close();
        UpdateWinRate();
    }
}

void GameRecord::SaveFile(QFile *file){
    //保存
    if(file->open(QIODevice::WriteOnly)){
        QDataStream out(file);
        out << this->Easy.WinNum;
        out << this->Easy.LostNum;
        out << this->Easy.FastestTime;

        out << this->Middle.WinNum;
        out << this->Middle.LostNum;
        out << this->Middle.FastestTime;

        out << this->Hard.WinNum;
        out << this->Hard.LostNum;
        out << this->Hard.FastestTime;

        file->close();
    }
}

void GameRecord::Reset(){
    this->Easy = {0,0,0,0};
    this->Middle = {0,0,0,0};
    this->Hard = {0,0,0,0};

}

void GameRecord::ShowRecord(Record re){
    QString text;
    text = QString("最快记录：%1s\n胜场：%2\n败场：%3\n胜率：%4\%").arg(re.FastestTime).arg(re.WinNum).arg(re.LostNum).arg(QString::number(re.WinRate,'f',1));
    this->ui->label->setText(text);
    this->update();
}

void GameRecord::UpdateWinNum(GameMode mode){
    if(mode == GameMode::Easy){
        this->Easy.WinNum++;
        this->Easy.WinRate = 100*this->Easy.WinNum/(this->Easy.WinNum+this->Easy.LostNum);
    }else if(mode == GameMode::Middle){
        this->Middle.WinNum++;
        this->Middle.WinRate = 100*this->Middle.WinNum/(this->Middle.WinNum+this->Middle.LostNum);
    }else if(mode == GameMode::Hard){
        this->Hard.WinNum++;
        this->Hard.WinRate = 100*this->Hard.WinNum/(this->Hard.WinNum+this->Hard.LostNum);
    }
}

void GameRecord::UpdateLostNum(GameMode mode){
    if(mode == GameMode::Easy){
        this->Easy.LostNum++;
        this->Easy.WinRate = 100*this->Easy.WinNum/(this->Easy.WinNum+this->Easy.LostNum);
    }else if(mode == GameMode::Middle){
        this->Middle.LostNum++;
        this->Middle.WinRate = 100*this->Middle.WinNum/(this->Middle.WinNum+this->Middle.LostNum);
    }else if(mode == GameMode::Hard){
        this->Hard.LostNum++;
        this->Hard.WinRate = 100*this->Hard.WinNum/(this->Hard.WinNum+this->Hard.LostNum);
    }
}

void GameRecord::UpdateTime(int time ,GameMode mode){
    if(mode == GameMode::Easy){
        if(time<this->Easy.FastestTime||Easy.FastestTime==0)
            this->Easy.FastestTime = time;
    }else if(mode == GameMode::Middle){
        if(time<this->Middle.FastestTime||Middle.FastestTime==0)
            this->Middle.FastestTime = time;
    }else if(mode == GameMode::Hard){
        if(time<this->Hard.FastestTime||Hard.FastestTime==0)
            this->Hard.FastestTime = time;
    }
}

void GameRecord::setDefault(){
    this->ui->easy->click();
}

void GameRecord::UpdateWinRate(){
    if(this->Easy.WinNum==0){
        this->Easy.WinRate = 0.0;
    }else {
        this->Easy.WinRate = 100*this->Easy.WinNum/(this->Easy.WinNum+this->Easy.LostNum);
    }

    if(this->Middle.WinNum==0){
        this->Middle.WinRate = 0.0;
    }else {
        this->Middle.WinRate = 100*this->Middle.WinNum/(this->Middle.WinNum+this->Middle.LostNum);
    }

    if(this->Hard.WinNum==0){
        this->Hard.WinRate = 0.0;
    }else {
        this->Hard.WinRate = 100*this->Hard.WinNum/(this->Hard.WinNum+this->Hard.LostNum);
    }
}
