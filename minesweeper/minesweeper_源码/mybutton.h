
#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QPushButton>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>
#include <QRect>

class mybutton : public QPushButton
{
    Q_OBJECT
private:
    bool enable;
public:
    using QPushButton::QPushButton;
    mybutton(QPushButton* parent = nullptr);
    void setEnable(bool enable);

signals:
    void entered();
    void left();
    void leftButtonClicked();
    void rightButtonClicked();
    void leftButtonReleased();
    void rightAndleftClicked();
    void rightOrleftRelease();

protected:
    void mousePressEvent(QMouseEvent *event) override{
        if (event->button() == Qt::LeftButton){
            emit leftButtonClicked();
        }
        if (event->button() == Qt::RightButton){
            emit rightButtonClicked();
        }
        if (event->buttons() == (Qt::LeftButton | Qt::RightButton)){
            emit rightAndleftClicked();
        }
        QPushButton::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent* event) override
    {
        if (event->button() == Qt::LeftButton) {
            emit leftButtonReleased();
        }
        if (event->button() == Qt::LeftButton||event->button() == Qt::RightButton){
            emit rightOrleftRelease();
        }
        QPushButton::mouseReleaseEvent(event);
    }

    void enterEvent(QEvent *event) override
    {
        emit entered();
        QPushButton::enterEvent(event);
    }


    void leaveEvent(QEvent *event) override
    {
        QPushButton::leaveEvent(event);
        emit left();
    }


    void mouseMoveEvent(QMouseEvent *event) override{
        return;
    }

};

#endif // MYBUTTON_H
