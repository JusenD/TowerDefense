#include "enemy.h"
#include<QPainter>
#include<QTimer>
#include<QPropertyAnimation>
#include"map.h"
#include<QMovie>
#include<mainwindow.h>

//所有敌人共用减少生命值函数
void Enemy::health_decrease(int n, int time){
    QTimer::singleShot(time, this, [=](){
        if(health>n) health-=n;
        else if(!dead){die(); dead = true;}
    });
}

void cut_off(QLabel* gif, int time){
    QTimer::singleShot(time, [=](){
        delete gif;
    });
}

bool Enemy::is_fly(){ return this->fly; }

bool Enemy::is_ground(){ return this->ground; };

Daida::Daida(QWidget *parent, Single_Path* path, Map* map)
{
    this->setParent(parent);
    this->parent = (MainWindow*)parent;
    this->this_path = path;
    this->resize(70, 70);
    this->map = map;
    x_now = path->way[0].x*70 + 70;
    y_now = path->way[0].y*70;
    this->move(x_now, y_now);
    this->health = 200;
}

void Daida::start_move(){
    move_once();
    //每隔2s进行一次递归
    QTimer::singleShot(2500, this, [=](){
        if(can_move) start_move();
    });
}

void Daida::stop_move(){
    if(!can_move){
        can_move = false;
        if(!block_now->empty()){
            attack();
        }
        //每隔2.5s进行一次递归攻击
        QTimer::singleShot(2500, this, [=](){
            if(block_now->empty()){
                can_move = true;
                start_move();
            }
            else {
                stop_move();
            }
        });
    }
}

void Daida::move_once(){
    step++;
    if(step == this_path->way.size()){
        can_move = false;
        emit defeat();
    }
    else{
        block_now = map->all_block[this_path->way[step].y * this->parent->the_map->get_colomn() + this_path->way[step].x];
        if(!block_now->empty()){
            QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
            //设置时间间隔
            animation->setDuration(1300);
            //设置起始位置
            animation->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
            x_now = this_path->way[step].x*70 + 35;
            y_now = this_path->way[step].y*70;
            //设置结束位置
            animation->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
            animation->setEasingCurve(QEasingCurve::InOutCubic);
            animation->start();
            can_move = false;
            QTimer::singleShot(1800, this, [=](){
                stop_move();
            });

        }
        else{
            QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
            //设置时间间隔
            animation->setDuration(1300);
            //设置起始位置
            animation->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
            x_now = this_path->way[step].x*70;
            y_now = this_path->way[step].y*70;
            //设置结束位置
            animation->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
            animation->setEasingCurve(QEasingCurve::InOutCubic);
            animation->start();
        }
    }
}

void Daida::attack(){       //一次攻击，攻击频率为 hz
    Defender* target = this->block_now->defender_in()->back();
    target->health_decrease(50, 250);
    //设置攻击动画
    QPropertyAnimation *animation1 = new QPropertyAnimation(this, "geometry");
    //设置时间间隔
    animation1->setDuration(250);
    //设置起始位置
    animation1->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
    //设置结束位置
    animation1->setEndValue(QRect(x_now - 4, y_now - 7, this->width(), this->height()));
    animation1->setEasingCurve(QEasingCurve::InOutCubic);
    animation1->start();
    QTimer::singleShot(250, this, [=](){
        QPropertyAnimation *animation2 = new QPropertyAnimation(this, "geometry");
        //设置时间间隔
        animation2->setDuration(150);
        //设置起始位置
        animation2->setStartValue(QRect(x_now - 4, y_now - 7, this->width(), this->height()));
        //设置结束位置
        animation2->setEndValue(QRect(x_now - 7, y_now, this->width(), this->height()));
        animation2->setEasingCurve(QEasingCurve::InOutCubic);
        animation2->start();
        QTimer::singleShot(150, this, [=](){
            QPropertyAnimation *animation3 = new QPropertyAnimation(this, "geometry");
            //设置时间间隔
            animation3->setDuration(150);
            //设置起始位置
            animation3->setStartValue(QRect(x_now - 7, y_now, this->width(), this->height()));
            //设置结束位置
            animation3->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
            animation3->setEasingCurve(QEasingCurve::InOutCubic);
            animation3->start();
        });
    });
}

void Daida::die(){
    can_move = false;
    //在所有敌人中删除该敌人
    vector<Enemy*>::iterator itor = map->all_enemy.begin();
    for(itor = map->all_enemy.begin(); itor != map->all_enemy.end(); itor++){
        if(*itor == this){
            itor = map->all_enemy.erase(itor);
            break;
        }
    }
    this->~Daida();
}


void Daida::paintEvent(QPaintEvent *){
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/daida.png");
    painter.drawPixmap(0, 0, 70, 70, pix);
}


//骷髅兵
Skeleton::Skeleton(QWidget *parent, Single_Path* path, Map* map){
    this->setParent(parent);
    this->parent = (MainWindow*)parent;
    this->this_path = path;
    this->resize(60, 90);
    this->map = map;
    x_now = path->way[0].x*70 + 70;
    y_now = path->way[0].y*70 - 30;
    this->move(x_now, y_now);
    this->health = 500;
    //初始化gif
    gif = new QLabel;
    gif->setParent(parent);
    gif->resize(60, 90);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->setScaledContents(true);
    gif->hide();
    gif->move(x_now, y_now);
}

void Skeleton::start_move(){
    move_once();
    //每隔3s进行一次递归
    QTimer::singleShot(3000, this, [=](){
        if(can_move) start_move();
    });
}

void Skeleton::stop_move(){
    movie = new QMovie(":/res/SkeletonIdle.gif");
    gif->setMovie(movie);
    movie->start();
    gif->show();
    if(!can_move){
        can_move = false;
        QTimer::singleShot(1500, this, [=](){
            if(!block_now->empty()) attack();
        });
        //每隔3s进行一次递归攻击
        QTimer::singleShot(3200, this, [=](){
            gif->move(this->x()+4, this->y());
            gif->resize(60, 90);
            if(block_now->empty()){
                can_move = true;
                start_move();
            }
            else {
                stop_move();
            }
        });
    }
}

void Skeleton::move_once(){//3s完成线性的行走
    movie = new QMovie(":/res/SkeletonWalk.gif");
    gif->setMovie(movie);
    movie->start();
    gif->show();
    step++;
    if(step == this_path->way.size()){
        can_move = false;
        emit defeat();
    }
    else{
        block_now = map->all_block[this_path->way[step].y * this->parent->the_map->get_colomn() + this_path->way[step].x];
        animation = new QPropertyAnimation(this, "geometry");
        animation2 = new QPropertyAnimation(gif, "geometry");
        //设置起始位置
        animation->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
        animation2->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
        //设置终止位置
        if(!block_now->empty()){
            //设置时间间隔
            animation->setDuration(500);
            animation2->setDuration(2500);
            x_now = this_path->way[step].x*70 + 50;
            y_now = this_path->way[step].y*70 - 30;
            animation->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
            animation->setEasingCurve(QEasingCurve::Linear);
            animation->start();
            animation2->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
            animation2->setEasingCurve(QEasingCurve::Linear);
            animation2->start();
            can_move = false;
            QTimer::singleShot(2800, this, [=](){
                stop_move();
            });
        }
        else{
            //设置时间间隔
            animation->setDuration(500);
            animation2->setDuration(3000);
            x_now = this_path->way[step].x*70;
            y_now = this_path->way[step].y*70 - 30;
            //设置结束位置
            animation->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
            animation->setEasingCurve(QEasingCurve::Linear);
            animation->start();
            animation2->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
            animation2->setEasingCurve(QEasingCurve::Linear);
            animation2->start();
        }
    }
}

void Skeleton::attack(){
    Defender* target = this->block_now->defender_in()->back();
    target->health_decrease(150, 900);
    gif->resize(120, 100);
    gif->move(this->x()-50, this->y() - 10);
    movie = new QMovie(":/res/SkeletonAttack.gif");
    gif->setMovie(movie);
    movie->start();
    gif->show();
}

void Skeleton::die(){
    can_move = false;
//    gif->resize(60, 90);
    animation->stop();
    animation2->stop();
    vector<Enemy*>::iterator itor = map->all_enemy.begin();
    for(itor = map->all_enemy.begin(); itor != map->all_enemy.end(); itor++){
        if(*itor == this){
            itor = map->all_enemy.erase(itor);
            break;
        }
    }
//    gif->resize(60, 90);
    movie = new QMovie(":/res/SkeletonDead.gif");
    movie->start();
    gif->setMovie(movie);
    gif->show();
    cut_off(gif, 1600);
    this->~Skeleton();
}


//蝙蝠
Bat::Bat(QWidget *parent, Single_Path* path, Map* map){
    this->setParent(parent);
    this->parent = (MainWindow*)parent;
    this->this_path = path;
    this->resize(70, 100);
    this->map = map;
    x_now = path->way[0].x*70 + 70;
    y_now = path->way[0].y*70 - 30;
    this->move(x_now, y_now);
    this->health = 100;
    //初始化gif
    gif = new QLabel;
    gif->setParent(parent);
    gif->resize(90, 90);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->setScaledContents(true);
    gif->hide();
    gif->move(x_now, y_now);
}

void Bat::start_move(){
    move_once();
    //每隔1.5s进行一次递归
    QTimer::singleShot(1500, this, [=](){
        if(can_move) start_move();
    });
}

void Bat::stop_move(){
    movie = new QMovie(":/res/BatFly.gif");
    gif->setMovie(movie);
    movie->start();
    gif->show();
    if(!can_move){
        can_move = false;
        QTimer::singleShot(1600, this, [=](){
            if(!block_now->empty()) attack();
        });
        //每隔2s进行一次递归攻击
        QTimer::singleShot(2000, this, [=](){
            if(block_now->empty()){
                can_move = true;
                start_move();
            }
            else {
                stop_move();
            }
        });
    }
}

void Bat::move_once(){//3s完成线性的行走
    movie = new QMovie(":/res/BatFly.gif");
    gif->setMovie(movie);
    movie->start();
    gif->show();
    step++;
    if(step == this_path->way.size()){
        can_move = false;
        emit defeat();
    }
    else{
        block_now = map->all_block[this_path->way[step].y * this->parent->the_map->get_colomn() + this_path->way[step].x];
        animation = new QPropertyAnimation(this, "geometry");
        animation2 = new QPropertyAnimation(gif, "geometry");
        //设置起始位置
        animation->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
        animation2->setStartValue(QRect(x_now, y_now, gif->width(), gif->height()));
        //设置终止位置
        if(!block_now->empty()){
            //设置时间间隔
            animation->setDuration(1000);
            animation2->setDuration(2000);
            x_now = this_path->way[step].x*70 + 35;
            y_now = this_path->way[step].y*70 - 30;
            animation->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
            animation->setEasingCurve(QEasingCurve::Linear);
            animation->start();
            animation2->setEndValue(QRect(x_now, y_now, gif->width(), gif->height()));
            animation2->setEasingCurve(QEasingCurve::Linear);
            animation2->start();
            can_move = false;
            QTimer::singleShot(2300, this, [=](){
                stop_move();
            });
        }
        else{
            //设置时间间隔
            animation->setDuration(800);
            animation2->setDuration(1500);
            x_now = this_path->way[step].x*70;
            y_now = this_path->way[step].y*70 - 30;
            //设置结束位置
            animation->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
            animation->setEasingCurve(QEasingCurve::Linear);
            animation->start();
            animation2->setEndValue(QRect(x_now, y_now, gif->width(), gif->height()));
            animation2->setEasingCurve(QEasingCurve::Linear);
            animation2->start();
        }
    }
}

void Bat::attack(){
    Defender* target = this->block_now->defender_in()->back();
    target->health_decrease(50, 300);
    movie = new QMovie(":/res/BatAttack.gif");
    gif->setMovie(movie);
    movie->start();
    gif->show();
}

void Bat::die(){
    can_move = false;
    animation->stop();
    animation2->stop();
    vector<Enemy*>::iterator itor = map->all_enemy.begin();
    for(itor = map->all_enemy.begin(); itor != map->all_enemy.end(); itor++){
        if(*itor == this){
            itor = map->all_enemy.erase(itor);
            break;
        }
    }
    movie = new QMovie(":/res/BatDeath.gif");
    movie->start();
    gif->setMovie(movie);
    gif->show();
    cut_off(gif, 1200);
    this->~Bat();
}










