#include "healthbar.h"
#include<QPainter>
#include<QDebug>

healthBar::healthBar(Enemy* parent) : QLabel(parent->get_gif())
{
    this->setParent(parent->get_gif());
    this->setFixedSize(1000, 10);
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->type = 0;
    this->enemy_parent = parent;
    this->total = parent->get_original_health();
    startTimer(100);
}

healthBar::healthBar(Defender* parent) : QLabel(parent->get_gif())
{
    this->setParent(parent->get_gif());
    this->setFixedSize(1000, 10);
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->type = 1;
    this->defender_parent = parent;
    this->total = parent->get_original_health();
    startTimer(100);
}

void healthBar::timerEvent(QTimerEvent* event){
    this->update();
}

void healthBar::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    float rate;
    if(type == 0){
 //     qDebug()<< enemy_parent->get_gif()->x()<<enemy_parent->get_gif()->y();
        QBrush red_brush(QColor("#CD2626"));
        painter.setBrush(red_brush);
        rate = 1.0 * enemy_parent->get_health()/total;
    }
    else{
        QBrush green_brush(QColor("#00FF00"));
        painter.setBrush(green_brush);
        rate = 1.0 * defender_parent->get_health()/total;
    }
    painter.drawRect(this->x(), this->y(), rate * 45, 3);
}
