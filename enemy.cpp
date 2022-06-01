#include "enemy.h"
#include<QPainter>
#include<QTimer>
#include<QPropertyAnimation>
#include"map.h"
#include<QMovie>
#include<mainwindow.h>
#include"healthbar.h"
#include<QTime>
#include<QDebug>

bool Enemy::on_delete = false;
bool Enemy::all_enemy_access = true;
//单独的删去敌人函数，避免同时删除bug
void Enemy::delete_enemy(){
    if(all_enemy_access){
        all_enemy_access = false;
        vector<Enemy*>::iterator itor = map->all_enemy.begin();
        for(itor = map->all_enemy.begin(); itor != map->all_enemy.end(); itor++){
            if(*itor == this){
                itor = map->all_enemy.erase(itor);
                break;
            }
        }
        all_enemy_access = true;
    }
    else{
        QTimer::singleShot(3, this, [=](){
            delete_enemy();
        });
    }
}

//所有敌人共用减少生命值函数
void Enemy::health_decrease(int n, int time){
    QTimer::singleShot(time, this, [=](){
        if(health>n) health-=n;
        else if(!has_dead){health = 0 ; die(); has_dead = true;}
    });
}

//buff
void Enemy::buff(){
    if(!on_buff){
        this->damage *= 1.2;
        this->health += 20;
        on_buff = true;
        buff_gif = new QLabel(gif);
        buff_gif->resize(60, 90);
        buff_gif->move(0, 10);
        buff_gif->setScaledContents(true);
        buff_movie = new QMovie(":/res/on_buffing.gif");
        buff_movie->start();
        buff_gif->setMovie(buff_movie);
        buff_gif->show();
        QTimer::singleShot(800, this, [=](){
            QMovie* former = buff_movie;
            buff_movie = new QMovie(":/res/on_buff.gif");
            former->deleteLater();
            buff_gif->resize(60, 20);
            buff_gif->move(5, 80);
            buff_movie->start();
            buff_gif->setMovie(buff_movie);
            buff_gif->show();
        });
    }
}

void Enemy::cut_off(QLabel* gif,QMovie* movie, int time){
    QTimer::singleShot(time, [=](){
        if(!on_delete){
            if(gif) gif->deleteLater();
        }
        if (movie) movie->deleteLater();
    });
}

bool Enemy::is_fly(){ return this->fly; }

bool Enemy::is_ground(){ return this->ground; };

int Enemy::get_x(){
    return this->x();
}

int Enemy::get_y(){
    return this->y();
}

int Enemy::get_health(){
    return this->health;
}

int Enemy::get_original_health(){
    return this->original_health;
}

QLabel* Enemy::get_gif(){
    return this->gif;
}

void Enemy::mouseReleaseEvent(QMouseEvent* event) {
    if(!detail_out){
        detail = new Detail(parent, this->get_gif(), &this->health, &this->damage, this->original_health, this->original_damage);
        detail->appear();
        detail_out = true;
        grabMouse();
    }
    else {
        detail->disappear();
        detail_out = false;
        releaseMouse();
    }
}

void Enemy::end(){
    this->map->decrease_health(this->health);
    this->die();
}

void Enemy::stop(){
    unfinished = false;
}

Daida::Daida(QWidget *parent, int which_path, Map* map, int step)
{
    this->setParent(parent);
    this->parent = (MainWindow*)parent;
    this->this_path = &(*map->path)[which_path];
    this->step = step -1;
    this->resize(70, 70);
    this->map = map;
    x_now = this_path->way[step].x*70 + 70;
    y_now = this_path->way[step].y*70;
    this->move(x_now, y_now);
    //初始化gif(为血条作准备)
    gif = new QLabel;
    gif->setParent(parent);
    gif->setFixedSize(60, 90);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->show();
    gif->move(x_now, y_now - 20);
    //初始化血条
    this->health = 200;
    this->original_health = health;
    this->health_bar = new healthBar(this);
    health_bar->move(6, 8);
    this->damage = 50;
    this->original_damage = damage;
    //初始化动画
    animation = new QPropertyAnimation(this, "geometry");
    animation1 = new QPropertyAnimation(this, "geometry");
    animation2 = new QPropertyAnimation(this, "geometry");
    animation3 = new QPropertyAnimation(this, "geometry");
    animation4 = new QPropertyAnimation(this->gif, "geometry");
    //初始化时钟
    move_clk = new QTimer(this);
    attack_clk = new QTimer(this);
    //每隔2.5s走一步，2.5s攻击一次
    move_clk->setInterval(2500);
    attack_clk->setInterval(2500);
    connect(move_clk, &QTimer::timeout, this, &Daida::move_once);
    connect(attack_clk, &QTimer::timeout, this, &Daida::attack);
}

void Daida::start_move(){
    health_bar->show();
    move_once();
    move_clk->start();
}

void Daida::move_once(){
    //游戏结束所有操作中断
    if(unfinished){
        step++;
        if(step >= this_path->way.size()){
            can_move = false;
    //        defeat();
            return end();
        }
        else{
            if(block_now) block_now->pop_enemy(this);
            block_now = map->all_block[this_path->way[step].y * this->parent->the_map->get_colomn() + this_path->way[step].x];
            block_now->push_an_enemy(this);
            animation->stop();
            animation1->stop();
            animation2->stop();
            animation3->stop();
            animation4->stop();
            if(!block_now->empty()){
                //停止移动
                move_clk->stop();
                //设置时间间隔
                animation->setDuration(1300);
                animation4->setDuration(1300);
                //设置起始位置
                animation->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
                animation4->setStartValue(QRect(x_now, y_now- 20, this->width(), this->height()));
                x_now = this_path->way[step].x*70 + 35;
                y_now = this_path->way[step].y*70;
                //设置结束位置
                animation->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
                animation->setEasingCurve(QEasingCurve::InOutCubic);
                animation->start();
                animation4->setEndValue(QRect(x_now, y_now - 20, this->width(), this->height()));
                animation4->setEasingCurve(QEasingCurve::InOutCubic);
                animation4->start();
                can_move = false;
                QTimer::singleShot(1800, this, [=](){
                    can_move = false;
                    move_clk->stop();
                    attack_clk->start();
                });

            }
            else{
                //设置时间间隔
                animation->setDuration(1300);
                animation4->setDuration(1300);
                //设置起始位置
                animation->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
                animation4->setStartValue(QRect(x_now, y_now- 20, this->width(), this->height()));
                x_now = this_path->way[step].x*70;
                y_now = this_path->way[step].y*70;
                //设置结束位置
                animation->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
                animation->setEasingCurve(QEasingCurve::InOutCubic);
                animation->start();
                animation4->setEndValue(QRect(x_now, y_now- 20, this->width(), this->height()));
                animation4->setEasingCurve(QEasingCurve::InOutCubic);
                animation4->start();
            }
        }
    }
}

void Daida::attack(){       //一次攻击，攻击频率为 hz
    //游戏结束所有操作中断
    if(unfinished){
        if(block_now->empty()){
            attack_clk->stop();
            move_clk->start();
            return move_once();
        }
        else{
            Defender* target = this->block_now->defender_in()->back();
            target->health_decrease(damage, 250);
            //设置攻击动画
            //设置时间间隔
            animation1->setDuration(250);
            //设置起始位置
            animation1->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
            //设置结束位置
            animation1->setEndValue(QRect(x_now - 4, y_now - 7, this->width(), this->height()));
            animation1->setEasingCurve(QEasingCurve::InOutCubic);
            animation1->start();
            QTimer::singleShot(250, this, [=](){
                //设置时间间隔
                animation2->setDuration(150);
                //设置起始位置
                animation2->setStartValue(QRect(x_now - 4, y_now - 7, this->width(), this->height()));
                //设置结束位置
                animation2->setEndValue(QRect(x_now - 7, y_now, this->width(), this->height()));
                animation2->setEasingCurve(QEasingCurve::InOutCubic);
                animation2->start();
                QTimer::singleShot(150, this, [=](){
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
    }
}

void Daida::die(){
    can_move = false;
    move_clk->stop();
    attack_clk->stop();
    animation->deleteLater();
    animation1->deleteLater();
    animation2->deleteLater();
    animation3->deleteLater();
    animation4->deleteLater();
    //在所有敌人中删除该敌人
    delete_enemy();
    this->health_bar->deleteLater();
    if(detail_out){detail->disappear();}
    this->gif->deleteLater();
    this->deleteLater();
}

void Daida::delete_now() {
    move_clk->stop();
    attack_clk->stop();
    animation->deleteLater();
    animation1->deleteLater();
    animation2->deleteLater();
    animation3->deleteLater();
    animation4->deleteLater();
    this->health_bar->deleteLater();
    if(detail_out){detail->deleteLater();}
    this->gif->deleteLater();
    this->deleteLater();
}

void Daida::paintEvent(QPaintEvent *){
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/daida.png");
    painter.drawPixmap(0, 0, 70, 70, pix);
}


//骷髅兵
Skeleton::Skeleton(QWidget *parent, int which_path, Map* map, int step){
    this->setParent(parent);
    this->parent = (MainWindow*)parent;
    this->this_path = &(*map->path)[which_path];
    this->step = step -1;
    this->setFixedSize(80, 70);
    this->map = map;
    x_now = this_path->way[step].x*70 + 70;
    y_now = this_path->way[step].y*70 - 30;
    this->move(x_now, y_now);
    this->health = 500;
    this->original_health = health;
    //初始化gif
    gif = new QLabel;
    gif->setParent(parent);
    gif->setFixedSize(120, 100);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->setScaledContents(true);
    gif->hide();
    gif->move(this->x()-50, this->y() - 10);
    movie = new QMovie(":/res/SkeletonWalk.gif");
    //初始化血条
    this->health_bar = new healthBar(this);
    health_bar->move(30, 8);
    this->damage = 150;
    this->original_damage = damage;
    //初始化animation
    animation = new QPropertyAnimation(this, "geometry");
    animation2 = new QPropertyAnimation(gif, "geometry");
    //初始化时钟
    move_clk = new QTimer(this);
    attack_clk = new QTimer(this);
    //每隔3s走一步，3s攻击一次
    move_clk->setInterval(3000);
    attack_clk->setInterval(3000);
    connect(move_clk, &QTimer::timeout, this, &Skeleton::move_once);
    connect(attack_clk, &QTimer::timeout, this, &Skeleton::attack);
}

void Skeleton::start_move(){
    health_bar->show();
    move_once();
    move_clk->start();
}

void Skeleton::move_once(){//3s完成线性的行走
    //游戏结束所有操作中断
    if(unfinished){
        gif->setMovie(movie);
        movie->start();
        gif->show();
        step++;
        if(step >= this_path->way.size()){
            can_move = false;
    //        defeat();
            return end();
        }
        else{
            if(block_now) block_now->pop_enemy(this);
            block_now = map->all_block[this_path->way[step].y * this->parent->the_map->get_colomn() + this_path->way[step].x];
            block_now->push_an_enemy(this);
            animation->stop();
            animation2->stop();
            //设置起始位置
            animation->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
            animation2->setStartValue(QRect(x_now-50, y_now-10, this->width(), this->height()));
            //设置终止位置
            if(!block_now->empty()){
                move_clk->stop();
                //设置时间间隔
                animation->setDuration(1000);
                animation2->setDuration(2500);
                x_now = this_path->way[step].x*70 + 50;
                y_now = this_path->way[step].y*70 - 30;
                animation->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
                animation->setEasingCurve(QEasingCurve::Linear);
                animation->start();
                animation2->setEndValue(QRect(x_now-50, y_now-10, this->width(), this->height()));
                animation2->setEasingCurve(QEasingCurve::Linear);
                animation2->start();
                can_move = false;
                QTimer::singleShot(2800, this, [=](){
                    QMovie* former = movie;
                    movie = new QMovie(":/res/SkeletonIdle.gif");
                    former->deleteLater();
                    gif->setMovie(movie);
                    movie->start();
                    gif->show();
                    attack_clk->start();
                });
            }
            else{
                //设置时间间隔
                animation->setDuration(1500);
                animation2->setDuration(3000);
                x_now = this_path->way[step].x*70;
                y_now = this_path->way[step].y*70 - 30;
                //设置结束位置
                animation->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
                animation->setEasingCurve(QEasingCurve::Linear);
                animation->start();
                animation2->setEndValue(QRect(x_now-50, y_now-10, this->width(), this->height()));
                animation2->setEasingCurve(QEasingCurve::Linear);
                animation2->start();
            }
        }
    }
}

void Skeleton::attack(){
    //游戏结束所有操作中断
    if(unfinished){
        if(block_now->empty()){
            can_move = true;
            attack_clk->stop();
            move_clk->start();
            QMovie* former = movie;
            movie = new QMovie(":/res/SkeletonWalk.gif");
            former->deleteLater();
            return move_once();
        }
        else{
            Defender* target = this->block_now->defender_in()->back();
            target->health_decrease(damage, 800);
            QMovie* former = movie;
            movie = new QMovie(":/res/SkeletonAttack.gif");
            former->deleteLater();
            gif->setMovie(movie);
            movie->start();
            gif->show();
            QTimer::singleShot(1700, this, [=](){
                QMovie* former = movie;
                movie = new QMovie(":/res/SkeletonIdle.gif");
                former->deleteLater();
                gif->setMovie(movie);
                movie->start();
                gif->show();
            });
        }
    }
}

void Skeleton::die(){
    move_clk->stop();
    attack_clk->stop();
    can_move = false;
    animation->stop();
    animation2->stop();
    animation->deleteLater();
    animation2->deleteLater();
    this->health_bar->deleteLater();
    if(detail_out){detail->disappear();}
    delete_enemy();
    QMovie* former = movie;
    movie = new QMovie(":/res/SkeletonDead.gif");
    former->deleteLater();
    movie->start();
    gif->setMovie(movie);
    gif->show();
    cut_off(gif,movie, 1400);
    this->deleteLater();
}

void Skeleton::delete_now() {
    move_clk->stop();
    attack_clk->stop();
    can_move = false;
    animation->stop();
    animation2->stop();
    animation->deleteLater();
    animation2->deleteLater();
    this->health_bar->deleteLater();
    if(detail_out){detail->deleteLater();}
    movie->deleteLater();
    gif->deleteLater();
    this->deleteLater();
}

//蝙蝠
Bat::Bat(QWidget *parent, int which_path, Map* map, int step){
    this->setParent(parent);
    this->parent = (MainWindow*)parent;
    this->this_path = &(*map->path)[which_path];
    this->step = step-1;
    this->resize(70, 100);
    this->map = map;
    x_now = this_path->way[step].x*70 + 70;
    y_now = this_path->way[step].y*70 - 30;
    this->move(x_now, y_now);
    this->health = 100;
    this->original_health = health;
    //初始化gif
    gif = new QLabel;
    gif->setParent(parent);
    gif->resize(90, 90);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->setScaledContents(true);
    gif->hide();
    gif->move(x_now, y_now);
    movie = new QMovie(":/res/BatFly.gif");
    //初始化血条
    this->health_bar = new healthBar(this);
    health_bar->move(10, 7);
    this->damage = 50;
    this->original_damage = damage;
    //初始化animation
    animation = new QPropertyAnimation(this, "geometry");
    animation2 = new QPropertyAnimation(gif, "geometry");
    //初始化时钟
    move_clk = new QTimer(this);
    attack_clk = new QTimer(this);
    //每隔1.5s走一步，2s攻击一次
    move_clk->setInterval(1500);
    attack_clk->setInterval(2000);
    connect(move_clk, &QTimer::timeout, this, &Bat::move_once);
    connect(attack_clk, &QTimer::timeout, this, &Bat::attack);
}

void Bat::start_move(){
    health_bar->show();
    move_once();
    move_clk->start();
}

void Bat::move_once(){//3s完成线性的行走
    //游戏结束所有操作中断
    if(unfinished){
        gif->setMovie(movie);
        movie->start();
        gif->show();
        step++;
        if(step >= this_path->way.size()){
            can_move = false;
    //        defeat();
            return end();
        }
        else{
            if(block_now) block_now->pop_enemy(this);
            block_now = map->all_block[this_path->way[step].y * this->parent->the_map->get_colomn() + this_path->way[step].x];
            block_now->push_an_enemy(this);
            animation->stop();
            animation2->stop();
            //设置起始位置
            animation->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
            animation2->setStartValue(QRect(x_now, y_now, gif->width(), gif->height()));
            //设置终止位置
            if(!block_now->empty()){
                move_clk->stop();
                //设置时间间隔
                animation->setDuration(700);
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
                    attack_clk->start();
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
}

void Bat::attack(){
    //游戏结束所有操作中断
    if(unfinished){
        if(block_now->empty()){
            can_move = true;
            attack_clk->stop();
            move_clk->start();
            return move_once();
        }
        else{
            Defender* target = this->block_now->defender_in()->back();
            target->health_decrease(damage, 300);
            QMovie* former = movie;
            movie = new QMovie(":/res/BatAttack.gif");
            former->deleteLater();
            gif->setMovie(movie);
            movie->start();
            gif->show();
            QTimer::singleShot(400, this, [=](){
                QMovie* former = movie;
                movie = new QMovie(":/res/BatFly.gif");
                former->deleteLater();
                gif->setMovie(movie);
                movie->start();
                gif->show();
            });
        }
    }
}

void Bat::die(){
    move_clk->stop();
    attack_clk->stop();
    can_move = false;
    animation->stop();
    animation2->stop();
    animation->deleteLater();
    animation2->deleteLater();
    this->health_bar->deleteLater();
    if(detail_out){detail->disappear();}
    delete_enemy();
    QMovie* former = movie;
    movie = new QMovie(":/res/BatDeath.gif");
    former->deleteLater();
    movie->start();
    gif->setMovie(movie);
    gif->show();
    cut_off(gif,movie, 1100);
    this->deleteLater();
}

void Bat::delete_now() {
    move_clk->stop();
    attack_clk->stop();
    can_move = false;
    animation->stop();
    animation2->stop();
    animation->deleteLater();
    animation2->deleteLater();
    this->health_bar->deleteLater();
    if(detail_out){detail->deleteLater();}
    movie->deleteLater();
    gif->deleteLater();
}

BlackWitch::BlackWitch(QWidget *parent, int which_path, Map* map, int step){
    this->setParent(parent);
    this->parent = (MainWindow*)parent;
    this->this_path = &(*map->path)[which_path];
    this->which_path = which_path;
    this->step = step -1;
    this->resize(70, 90);
    this->map = map;
    x_now = this_path->way[step].x*70 + 70;
    y_now = this_path->way[step].y*70 - 20;
    this->move(x_now, y_now);
    this->health = 400;
    this->original_health = health;
    //初始化gif
    gif = new QLabel;
    gif->setParent(parent);
    gif->resize(70, 70);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->setScaledContents(true);
    gif->hide();
    gif->move(x_now, y_now);
    movie = new QMovie(":/res/BlackWitchFly.gif");
    //初始化血条
    this->health_bar = new healthBar(this);
    //初始化anmiation
    animation = new QPropertyAnimation(this, "geometry");
    animation2 = new QPropertyAnimation(gif, "geometry");
    //初始化时钟
    move_clk = new QTimer(this);
    attack_clk = new QTimer(this);
    //每隔4s走一步，7s召唤一次
    move_clk->setInterval(4000);
    attack_clk->setInterval(7000);
    connect(move_clk, &QTimer::timeout, this, &BlackWitch::move_once);
    connect(attack_clk, &QTimer::timeout, this, &BlackWitch::start_call);
}

void BlackWitch::start_move(){
    health_bar->show();
    gif->setMovie(movie);
    movie->start();
    gif->show();
    move_once();
    move_clk->start();
    attack_clk->start();
}

void BlackWitch::move_once(){
    //游戏结束所有操作中断
    if(unfinished){ 
        step++;
        if(step >= this_path->way.size()){
            can_move = false;
    //        defeat();
            return end();
        }
        else{
            if(block_now) block_now->pop_enemy(this);
            block_now = map->all_block[this_path->way[step].y * this->parent->the_map->get_colomn() + this_path->way[step].x];
            block_now->push_an_enemy(this);
            animation->stop();
            animation2->stop();
            //设置起始位置
            animation->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
            animation2->setStartValue(QRect(x_now, y_now, gif->width(), gif->height()));
            //设置终止位置
            //设置时间间隔
            animation->setDuration(2000);
            animation2->setDuration(4000);
            x_now = this_path->way[step].x*70;
            y_now = this_path->way[step].y*70 - 20;
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

void BlackWitch::start_call(){
    //游戏结束所有操作中断
    if(unfinished){
        //快到终点时不再召唤，不然会有未知bug
        if(step + 1 < this_path->way.size()){
            QMovie* former = movie;
            movie = new QMovie(":/res/BlackWitchCall.gif");
            former->deleteLater();
            gif->setMovie(movie);
            movie->start();
            gif->show();
            //0.3s后召唤蝙蝠
            QTimer::singleShot(300, this, [=](){
                this->parent->the_map->add_enemy(parent, which_path, Enemies::Bat, step);
            });
            QTimer::singleShot(500, this, [=](){
                QMovie* former = movie;
                movie = new QMovie(":/res/BlackWitchFly.gif");
                former->deleteLater();
                gif->setMovie(movie);
                movie->start();
                gif->show();
            });
        }
    }
}

void runaaaa(QLabel* gif, int time){
    QPropertyAnimation *run_away = new QPropertyAnimation(gif, "geometry");
    run_away->setStartValue(QRect(gif->x(), gif->y(), gif->width(), gif->height()));
    run_away->setDuration(1000);
    run_away->setEasingCurve(QEasingCurve::InQuint);
    run_away->setEndValue(QRect(gif->x(), -300, gif->width(), gif->height()));
    QTimer::singleShot(time, [=](){
        run_away->start();
    });
}

void BlackWitch::die(){
    move_clk->stop();
    attack_clk->stop();
    can_move = false;
    animation->stop();
    animation2->stop();
    animation->deleteLater();
    animation2->deleteLater();
    this->health_bar->deleteLater();
    if(detail_out){detail->disappear();}
    delete_enemy();
    QMovie* former = movie;
    movie = new QMovie(":/res/BlackWitchDead.gif");
    former->deleteLater();
    movie->start();
    gif->resize(178, 252);
    gif->move(gif->x() - 48, gif->y() - 10 -162);
    gif->setMovie(movie);
    gif->show();
    //设置逃离动画
    runaaaa(gif, 700);
    cut_off(gif,movie, 2200);
    this->deleteLater();
}

void BlackWitch::delete_now() {
    move_clk->stop();
    attack_clk->stop();
    can_move = false;
    animation->stop();
    animation2->stop();
    animation->deleteLater();
    animation2->deleteLater();
    this->health_bar->deleteLater();
    if(detail_out){detail->deleteLater();}
    movie->deleteLater();
    gif->deleteLater();
    this->deleteLater();
}

void BlackWitch::attack(){

}

Bot::Bot(QWidget *parent, int which_path, Map* map, int step){
    this->setParent(parent);
    this->parent = (MainWindow*)parent;
    this->this_path = &(*map->path)[which_path];
    this->step = step-1;
    this->setFixedSize(70, 70);
    this->map = map;
    x_now = this_path->way[step].x*70 + 70;
    y_now = this_path->way[step].y*70;
    this->move(x_now, y_now);
    this->health = 500;
    this->original_health = health;
    //初始化gif
    gif = new QLabel;
    gif->setParent(parent);
    gif->resize(315, 70);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->setScaledContents(true);
    gif->hide();
    gif->move(x_now - 215, y_now);
    movie = new QMovie(":/res/BotWalk.gif");
    gif->setMovie(movie);
    bullet = new QLabel(parent);
    QPixmap pix(":/res/bullet.png");
    bullet->setFixedSize(35, 5);
    bullet->setScaledContents(true);
    bullet->setAttribute(Qt::WA_TransparentForMouseEvents);
    bullet->hide();
    bullet->setPixmap(pix);
    //初始化血条
    this->health_bar = new healthBar(this);
    health_bar->move(115, 0);
    this->damage = 150;
    this->original_damage = damage;
    //初始化animation
    animation = new QPropertyAnimation(this, "geometry");
    animation2 = new QPropertyAnimation(gif, "geometry");
    animation3 = new QPropertyAnimation(bullet, "geometry");
    //初始化时钟
    move_clk = new QTimer(this);
    attack_clk = new QTimer(this);
    //每隔5s走一步，4s攻击一次
    move_clk->setInterval(5000);
    attack_clk->setInterval(4000);
    connect(move_clk, &QTimer::timeout, this, &Bot::move_once);
    connect(attack_clk, &QTimer::timeout, this, &Bot::attack);
}

void Bot::start_move(){
    health_bar->show();
    move_once();
    move_clk->start();
}

void Bot::move_once(){
    //游戏结束所有操作中断
    if(unfinished){
        movie->start();
        gif->show();
        step++;
        on_attack = false;
        if(step >= this_path->way.size()){
            can_move = false;
    //        defeat();
            return end();
        }
        else{
            if(block_now) block_now->pop_enemy(this);
            block_now = map->all_block[this_path->way[step].y * map->get_colomn() + this_path->way[step].x];
            block_now->push_an_enemy(this);
            animation->stop();
            animation2->stop();
            //设置起始位置
            animation->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
            animation2->setStartValue(QRect(x_now - 215, y_now, gif->width(), gif->height()));
            //设置终止位置
            if(!block_now->empty()){
                //设置时间间隔
                animation->setDuration(1000);
                animation2->setDuration(4500);
                x_now = this_path->way[step].x*70 + 35;
                y_now = this_path->way[step].y*70;
                animation->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
                animation->setEasingCurve(QEasingCurve::Linear);
                animation->start();
                animation2->setEndValue(QRect(x_now - 215, y_now, gif->width(), gif->height()));
                animation2->setEasingCurve(QEasingCurve::Linear);
                animation2->start();
                can_move = false;
                QTimer::singleShot(3000, this, [=](){
                    move_clk->stop();
                    attack_clk->start();
                });
            }
            else{
                //设置时间间隔
                animation->setDuration(1000);
                animation2->setDuration(5000);
                x_now = this_path->way[step].x*70;
                y_now = this_path->way[step].y*70;
                //设置结束位置
                animation->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
                animation->setEasingCurve(QEasingCurve::Linear);
                animation->start();
                animation2->setEndValue(QRect(x_now - 215, y_now, gif->width(), gif->height()));
                animation2->setEasingCurve(QEasingCurve::Linear);
                animation2->start();
                QTimer::singleShot(4000, this, [=](){
                    if(find_target()){
                        move_clk->stop();
                        attack_clk->start();
                        QTimer::singleShot(1000, this, [=](){
                            QMovie* former = movie;
                            movie = new QMovie(":/res/BotCharge.gif");
                            former->deleteLater();
                            gif->setMovie(movie);
                            movie->start();
                            gif->show();
                            on_attack = true;
                        });
                    }
                });
            }
        }
    }
}

Defender* Bot::find_target(){
    //搜索是否有可以攻击目标
    if(!this->block_now->empty()) return (*(block_now->defender_in())).front();
    for(int i = this_path->way[step].x - 1; i >= 0 && i >= this_path->way[step].x - 3; i--){
        for(int j = this_path->way[step].y - 1; j <= this_path->way[step].y + 1; j++){
            if(j < 0) continue;
            if(j == map->get_row()) break;
            if(!((*(map->all_block[j * map->get_colomn() + i]->defender_in())).empty())){
                return (*(map->all_block[j * map->get_colomn() + i]->defender_in())).front();
            }
        }
    }
    return nullptr;
}

void Bot::attack(){
    //游戏结束所有操作中断
    if(unfinished){
        Defender* target = find_target();
        if(!target){
            QMovie* former = movie;
            movie = new QMovie(":/res/BotWalk.gif");
            former->deleteLater();
            gif->setMovie(movie);
            movie->start();
            gif->show();
            can_move = true;
            attack_clk->stop();
            move_clk->start();
            return move_once();
        }
        else{
            animation3->setStartValue(QRect(x_now, y_now+10, bullet->width(), bullet->height()));
            animation3->setEndValue(QRect(target->x()+35, target->y()+35, bullet->width(), bullet->height()));
            animation->setEasingCurve(QEasingCurve::Linear);
            animation3->setDuration(100);
            bullet->show();
            animation3->start();
            QTimer::singleShot(100, bullet, [=](){
                bullet->hide();
            });
            target->health_decrease(damage, 100);
            QMovie* former = movie;
            movie = new QMovie(":/res/BotAttack.gif");
            former->deleteLater();
            gif->setMovie(movie);
            movie->start();
            gif->show();
            QTimer::singleShot(350, this, [=](){
                QMovie* former = movie;
                movie = new QMovie(":/res/BotCharge.gif");
                former->deleteLater();
                gif->setMovie(movie);
                movie->start();
                gif->show();
            });
        }
    }
}

void Bot::health_decrease(int n, int time){
    if(!invincible){
        QTimer::singleShot(time - 100, this, [=](){
            QTime time = QTime::currentTime();
            srand(time.second()*1000 + time.msec());
            int lucky_dog = rand()%10;
            if(lucky_dog < 10 * miss){
                invincible = true;
                bool attack_pause = false, move_pause = false;
                int move_remain = move_clk->remainingTime();
                int attack_remain = attack_clk->remainingTime();
                qDebug()<<"attack_pause:"<<attack_pause<<"move_pause:"<< move_pause <<"attack_remain:"<<attack_remain<<"move_remain:"<<move_remain;
                if(move_remain != -1 && move_remain < 500) {move_clk->stop(); move_pause = true;}
                if(attack_remain!= -1 && attack_remain < 500) {attack_clk->stop(); attack_pause = true;}
                if(attack_remain != -1) on_attack = true;
                QMovie* former = movie;
                movie = new QMovie(":/res/BotDashL.gif");
                former->deleteLater();
                gif->setMovie(movie);
                movie->start();
                gif->show();
                QTimer::singleShot(250, this, [=](){
                    QMovie* former = movie;
                    movie = new QMovie(":/res/BotDashR.gif");
                    former->deleteLater();
                    gif->setMovie(movie);
                    movie->start();
                    gif->show();
                });
                QTimer::singleShot(500, this, [=](){
                    invincible = false;
                    QMovie* former = movie;
                    if(on_attack) movie = new QMovie(":/res/BotCharge.gif");
                    else movie = new QMovie(":/res/BotWalk.gif");
                    former->deleteLater();
                    gif->setMovie(movie);
                    movie->start();
                    gif->show();
                    if(attack_pause) {
                            attack_clk->start(500);
                            QTimer::singleShot(510, this, [=](){
                                attack_clk->setInterval(4000);
                                attack_clk->start();
                            });
                    }
                    if(move_pause) {
                            move_clk->start(500);
                            QTimer::singleShot(510, this, [=](){
                                move_clk->setInterval(5000);
                                if(!on_attack) move_clk->start();
                            });
                    }
                });
            }
            else{
                QTimer::singleShot(90, this, [=](){
                    if(health>n) health-=n;
                    else if(!has_dead){die(); has_dead = true;}
                });
            }
        });
    }
}

void Bot::die(){
    move_clk->stop();
    attack_clk->stop();
    can_move = false;
    animation->stop();
    animation2->stop();
    animation->deleteLater();
    animation2->deleteLater();
    animation3->deleteLater();
    this->health_bar->deleteLater();
    if(detail_out){detail->disappear();}
    delete_enemy();
    QMovie* former = movie;
    movie = new QMovie(":/res/BotDead.gif");
    former->deleteLater();
    movie->start();
    gif->setMovie(movie);
    gif->show();
    cut_off(gif,movie, 500);
    bullet->deleteLater();
    this->deleteLater();
}

void Bot::delete_now(){
    move_clk->stop();
    attack_clk->stop();
    can_move = false;
    animation->stop();
    animation2->stop();
    animation->deleteLater();
    animation2->deleteLater();
    this->health_bar->deleteLater();
    if(detail_out){detail->deleteLater();}
    movie->deleteLater();
    gif->deleteLater();
}

//Buff法师
Plus::Plus(QWidget *parent, int which_path, Map* map, int step){
    this->setParent(parent);
    this->parent = (MainWindow*)parent;
    this->this_path = &(*map->path)[which_path];
    this->step = step-1;
    this->resize(70, 100);
    this->map = map;
    x_now = this_path->way[step].x*70 + 70;
    y_now = this_path->way[step].y*70;
    this->move(x_now, y_now);
    this->health = 200;
    this->original_health = health;
    //初始化gif
    gif = new QLabel;
    gif->setParent(parent);
    gif->resize(160, 100);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->setScaledContents(true);
    gif->hide();
    gif->move(x_now, y_now);
    movie = new QMovie(":/res/PlusWalk.gif");
    //初始化血条
    this->health_bar = new healthBar(this);
    health_bar->move(17, 5);
    this->damage = 0;
    this->original_damage = damage;
    //初始化animation
    animation = new QPropertyAnimation(this, "geometry");
    animation2 = new QPropertyAnimation(gif, "geometry");
    //初始化时钟
    move_clk = new QTimer(this);
    attack_clk = new QTimer(this);
    //每隔8s走一步，40s叠Buff一次
    move_clk->setInterval(8000);
    attack_clk->setInterval(30000);
    connect(move_clk, &QTimer::timeout, this, &Plus::move_once);
    connect(attack_clk, &QTimer::timeout, this, &Plus::enable_attack);
}

void Plus::start_move(){
    health_bar->show();
    move_once();
    move_clk->start();
    attack_clk->start();
}

void Plus::move_once(){//8s完成线性的行走
    //游戏结束所有操作中断
    if(unfinished){
        QMovie* former = movie;
        movie = new QMovie(":/res/PlusWalk.gif");
        former->deleteLater();
        gif->setMovie(movie);
        movie->start();
        gif->show();
        step++;
        if(step >= this_path->way.size()){
            can_move = false;
    //        defeat();
            return end();
        }
        else{
            animation->stop();
            animation2->stop();
            if(can_attack){
                step--;
                move_clk->stop();
                return attack();
            }
            if(block_now) block_now->pop_enemy(this);
            block_now = map->all_block[this_path->way[step].y * this->parent->the_map->get_colomn() + this_path->way[step].x];
            block_now->push_an_enemy(this);
            //设置起始位置
            animation->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
            animation2->setStartValue(QRect(x_now-20, y_now-35, gif->width(), gif->height()));
            //设置终止位置
            //设置时间间隔
            animation->setDuration(5000);
            animation2->setDuration(8000);
            x_now = this_path->way[step].x*70;
            y_now = this_path->way[step].y*70;
            //设置结束位置
            animation->setEndValue(QRect(x_now, y_now, this->width(), this->height()));
            animation->setEasingCurve(QEasingCurve::Linear);
            animation->start();
            animation2->setEndValue(QRect(x_now-20, y_now-35, gif->width(), gif->height()));
            animation2->setEasingCurve(QEasingCurve::Linear);
            animation2->start();
        }
    }
}

void Plus::enable_attack(){
    this->can_attack = true;
}

void Plus::attack(){
    //游戏结束所有操作中断
    if(unfinished){
        move_clk->stop();
        attack_clk->stop();
        QMovie* former = movie;
        movie = new QMovie(":/res/PlusWait.gif");
        former->deleteLater();
        gif->setMovie(movie);
        movie->start();
        gif->show();
        QTimer::singleShot(2000, this, [=](){
            QMovie* former = movie;
            movie = new QMovie(":/res/PlusBuff.gif");
            former->deleteLater();
            gif->setMovie(movie);
            movie->start();
            gif->show();
            QTimer::singleShot(1800, this, [=](){
                for(int i = step+1;i < this_path->way.size()-1 && i <= step + 5; i++){
                    for(auto enemy : *(map->all_block[this_path->way[i].y * this->parent->the_map->get_colomn() + this_path->way[i].x]->enemy_in())){
                        enemy->buff();
                    }
                }
            });
            QTimer::singleShot(2500, this, [=](){
                QMovie* former = movie;
                movie = new QMovie(":/res/PlusWait.gif");
                former->deleteLater();
                gif->setMovie(movie);
                movie->start();
                gif->show();
                attack_clk->start();
            });
            QTimer::singleShot(5000, this, [=](){
                move_clk->start();
                can_attack = false;
                move_once();
            });
        });
    }
}

void Plus::die(){
    move_clk->stop();
    attack_clk->stop();
    can_move = false;
    animation->stop();
    animation2->stop();
    animation->deleteLater();
    animation2->deleteLater();
    this->health_bar->deleteLater();
    if(detail_out){detail->disappear();}
    delete_enemy();
    QMovie* former = movie;
    movie = new QMovie(":/res/PlusDead.gif");
    former->deleteLater();
    movie->start();
    gif->resize(100, 190);
    gif->move(gif->x() + 25, gif->y() - 90);
    gif->setMovie(movie);
    gif->show();
    cut_off(gif,movie, 1600);
    this->deleteLater();
}

void Plus::delete_now() {
    move_clk->stop();
    attack_clk->stop();
    can_move = false;
    animation->stop();
    animation2->stop();
    animation->deleteLater();
    animation2->deleteLater();
    this->health_bar->deleteLater();
    if(detail_out){detail->deleteLater();}
    movie->deleteLater();
    gif->deleteLater();
}







