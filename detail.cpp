#include "detail.h"
#include <QTimer>
#include <QTimerEvent>

Detail::Detail(QWidget *parent,int x, int y, int* health, int* damage, int original_health, int original_damage) : QLabel(parent)
{
    this->setFixedSize(200, 25);
    attach_x = x;
    attach_y = y;
    this->health = health;
    this->original_health = original_health;
    this->damage = damage;
    this->original_damage = original_damage;
    health_text = new QLabel(this);
    damage_text = new QLabel(this);
    health_text->setFixedSize(70, 20);
    damage_text->setFixedSize(70, 20);
    health_text->move(20, 0);
    damage_text->move(120, 0);
    health_text->setAlignment(Qt::AlignCenter);
    damage_text->setAlignment(Qt::AlignCenter);
    health_text->setText((std::to_string(*health).append("/").append(std::to_string(this->original_health))).c_str());
    damage_text->setText((std::to_string(*damage).append("/").append(std::to_string(this->original_damage))).c_str());
    //装载背景图片
    QPixmap pix;
    pix.load(":/res/detail.png");
    this->setScaledContents(true);
    this->setPixmap(pix);
    //初始化弹出/消失动画
    detail_animation1 = new QPropertyAnimation(this, "geometry");
    detail_animation2 = new QPropertyAnimation(this, "geometry");
}

Detail::Detail(QWidget *parent, QLabel* follow, int* health, int* damage, int original_health, int original_damage, bool PLUS) : QLabel(parent)
{
    this->setFixedSize(200, 25);
    attach_x = PLUS ? follow->x() + follow->width() - 70 - 65 - 70 : follow->x() + follow->width() - 70 - 65;
    attach_y = follow->y() + follow->height();
    is_enemy = true;
    special_PLUS = PLUS;
    this->follow = follow;
    this->health = health;
    this->original_health = original_health;
    this->damage = damage;
    this->original_damage = original_damage;
    health_text = new QLabel(this);
    damage_text = new QLabel(this);
    health_text->setFixedSize(70, 20);
    damage_text->setFixedSize(70, 20);
    health_text->move(20, 0);
    damage_text->move(120, 0);
    health_text->setAlignment(Qt::AlignCenter);
    damage_text->setAlignment(Qt::AlignCenter);
    health_text->setText((std::to_string(*health).append("/").append(std::to_string(this->original_health))).c_str());
    damage_text->setText((std::to_string(*damage).append("/").append(std::to_string(this->original_damage))).c_str());
    //装载背景图片
    QPixmap pix;
    pix.load(":/res/detail.png");
    this->setScaledContents(true);
    this->setPixmap(pix);
    //初始化弹出/消失动画
    detail_animation1 = new QPropertyAnimation(this, "geometry");
    detail_animation2 = new QPropertyAnimation(this, "geometry");
    startTimer(100);
}

void Detail::appear(){
    if(is_enemy){
        detail_animation1->setStartValue(QRect(attach_x + 30, attach_y, 140, 5));
        detail_animation1->setEndValue(QRect(attach_x, attach_y, 200, 25));
    }
    else {
        detail_animation1->setStartValue(QRect(attach_x-35, attach_y+70, 140, 5));
        detail_animation1->setEndValue(QRect(attach_x-65, attach_y+70, 200, 25));
    }
    detail_animation1->setEasingCurve(QEasingCurve::OutElastic);
    detail_animation1->setDuration(200);
    this->show();
    detail_animation1->start();
    startTimer(100);
if(is_enemy) QTimer::singleShot(200, this, [=](){startTimer(30);});
}

void Detail::disappear(){
    on_disappear = true;
    if(is_enemy){
        detail_animation2->setStartValue(QRect(attach_x, attach_y, 200, 25));
        detail_animation2->setEndValue(QRect(attach_x + 65, attach_y, 140, 5));
        health_text->setText((std::to_string(*health).append("/").append(std::to_string(this->original_health))).c_str());
        damage_text->setText((std::to_string(*damage).append("/").append(std::to_string(this->original_damage))).c_str());
    }
    else{
        detail_animation2->setStartValue(QRect(attach_x-65, attach_y+70, 200, 25));
        detail_animation2->setEndValue(QRect(attach_x, attach_y+70, 140, 5));
    }
    detail_animation2->setEasingCurve(QEasingCurve::InCirc);
    detail_animation2->setDuration(200);
    detail_animation2->start();
    QTimer::singleShot(200, [=](){
        detail_animation1->deleteLater();
        detail_animation2->deleteLater();
        this->deleteLater();
    });
}

void Detail::timerEvent(QTimerEvent* event){
    if(!on_disappear){
        health_text->setText((std::to_string(*health).append("/").append(std::to_string(this->original_health))).c_str());
        damage_text->setText((std::to_string(*damage).append("/").append(std::to_string(this->original_damage))).c_str());
        if(is_enemy){
            attach_x = special_PLUS ? follow->x() + follow->width() - 70 - 65 - 70 : follow->x() + follow->width() - 70 - 65;
            attach_y = follow->y() + (follow->height());
            this->move(attach_x, attach_y);
        }
    }
}
