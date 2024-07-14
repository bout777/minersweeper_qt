
#include "mybutton.h"

mybutton::mybutton(QPushButton* parent) :QPushButton(parent)
{
    enable = true;
}

void mybutton::setEnable(bool enable)
{
    this->enable = enable;
}

