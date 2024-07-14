
#include "player.h"

player::player(QObject *parent)
    : QObject{parent}
{
    spread = new QSoundEffect(this);
    spread->setSource(QUrl::fromLocalFile(":/res/music/spread.wav"));
}

void player::play(){

}
