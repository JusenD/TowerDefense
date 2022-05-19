#include "enemy.h"
#include<QPainter>
#include<QTimer>
#include<QPropertyAnimation>
#include"map.h"
#include<QMovie>
#include<mainwindow.h>
#include"healthbar.h"

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
        else if(!dead){die(); dead = true;}
    });
}

void Enemy::cut_off(QLabel* gif, int time){
    QTimer::singleShot(time, [=](){
        if(!on_delete){
            if(gif) gif->deleteLater();
        }
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
    gif->resize(60, 90);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->show();
    gif->move(x_now, y_now - 20);
    //初始化血条
    this->health = 200;
    this->original_health = health;
    this->health_bar = new healthBar(this);
    health_bar->move(6, 8);
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
            block_now = map->all_block[this_path->way[step].y * this->parent->the_map->get_colomn() + this_path->way[step].x];
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
                    //保持血条的运动
                    animation4->setDuration(999999);
                    animation4->setStartValue(QRect(x_now, y_now- 20, this->width(), this->height()));
                    animation4->setEndValue(QRect(x_now+1, y_now- 20, this->width(), this->height()));
                    animation4->setEasingCurve(QEasingCurve::InOutCubic);
                    animation4->start();
 //                   stop_move();
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
                QTimer::singleShot(1800, this, [=](){
                    //保持血条的运动
                    animation4->setDuration(999999);
                    animation4->setStartValue(QRect(x_now, y_now- 20, this->width(), this->height()));
                    animation4->setEndValue(QRect(x_now+1, y_now- 20, this->width(), this->height()));
                    animation4->setEasingCurve(QEasingCurve::InOutCubic);
                    animation4->start();
                });
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
            target->health_decrease(50, 250);
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
    //在所有敌人中删除该敌人
    delete_enemy();
    this->health_bar->deleteLater();
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
    //初始化血条
    this->health_bar = new healthBar(this);
    health_bar->move(30, 8);
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
        QMovie* former = movie;
        movie = new QMovie(":/res/SkeletonWalk.gif");
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
            block_now = map->all_block[this_path->way[step].y * this->parent->the_map->get_colomn() + this_path->way[step].x];
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
            return move_once();
        }
        else{
            Defender* target = this->block_now->defender_in()->back();
            target->health_decrease(150, 800);
            QMovie* former = movie;
            movie = new QMovie(":/res/SkeletonAttack.gif");
            former->deleteLater();
            gif->setMovie(movie);
            movie->start();
            gif->show();
            QTimer::singleShot(1700, this, [=](){
                gif->resize(60, 90);
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
    this->health_bar->deleteLater();
    delete_enemy();
    QMovie* former = movie;
    movie = new QMovie(":/res/SkeletonDead.gif");
    former->deleteLater();
    movie->start();
    gif->setMovie(movie);
    gif->show();
    cut_off(gif, 1400);
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
    //初始化血条
    this->health_bar = new healthBar(this);
    health_bar->move(10, 7);
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
        QMovie* former = movie;
        movie = new QMovie(":/res/BatFly.gif");
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
            block_now = map->all_block[this_path->way[step].y * this->parent->the_map->get_colomn() + this_path->way[step].x];
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
                animation->setDuration(
                            800);
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
            target->health_decrease(50, 300);
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
    this->health_bar->deleteLater();
    delete_enemy();
    QMovie* former = movie;
    movie = new QMovie(":/res/BatDeath.gif");
    former->deleteLater();
    movie->start();
    gif->setMovie(movie);
    gif->show();
    cut_off(gif, 1100);
    this->deleteLater();
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
    QMovie* former = movie;
    movie = new QMovie(":/res/BlackWitchFly.gif");
    former->deleteLater();
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
            block_now = map->all_block[this_path->way[step].y * this->parent->the_map->get_colomn() + this_path->way[step].x];
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
    this->health_bar->deleteLater();
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
    cut_off(gif, 2200);
    this->deleteLater();
}

void BlackWitch::attack(){

}








