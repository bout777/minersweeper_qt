
#ifndef PLAYER_H
#define PLAYER_H


#include <QObject>
#include <QSoundEffect>


class player : public QObject
{
    Q_OBJECT
public:
    explicit player(QObject* parent = nullptr);
    void play();
signals:

private:
    QSoundEffect *spread;
};

#endif // PLAYER_H
