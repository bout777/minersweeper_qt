#ifndef GAMERECORD_H
#define GAMERECORD_H

#include <QDialog>
#include <QFile>
#include <QDataStream>
#include <QRadioButton>

namespace Ui {
class GameRecord;
}

struct Record {
    int WinNum;
    int LostNum;
    float WinRate;
    int FastestTime;
};

enum class GameMode{
    Easy,
    Middle,
    Hard,
    SelfDefine
};

class GameRecord : public QDialog
{
    Q_OBJECT
signals:
    void reset();

public:

    explicit GameRecord(QWidget *parent = nullptr);
    ~GameRecord();
    void UpdateTime(int,GameMode);
    void UpdateWinNum(GameMode);
    void UpdateLostNum(GameMode);
    void UpdateWinRate();
    void LoadFile(QFile*);
    void SaveFile(QFile*);
    void Reset();
    void ShowRecord(Record);
    void setDefault();
private:
    Record Easy;
    Record Middle;
    Record Hard;
    Ui::GameRecord *ui;
};

#endif // GAMERECORD_H
