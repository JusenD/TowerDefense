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

int Enemy::get_x(){
    return this->x();
}

int Enemy::get_y(){
    return this->y();
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
    this->step = step ? step : -1;
    this->resize(70, 70);
    this->map = map;
    x_now = this_path->way[step].x*70 + 70;
    y_now = this_path->way[step].y*70;
    this->move(x_now, y_now);
    this->health = 200;
    //初始化动画
    animation = new QPropertyAnimation(this, "geometry");
    animation1 = new QPropertyAnimation(this, "geometry");
    animation2 = new QPropertyAnimation(this, "geometry");
    animation3 = new QPropertyAnimation(this, "geometry");
}

void Daida::start_move(){
    move_once();
    //每隔2s进行一次递归
    QTimer::singleShot(2500, this, [=](){
        if(unfinished && can_move) start_move();
    });
}

void Daida::stop_move(){
    if(unfinished && !can_move){
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
    //游戏结束所有操作中断
    if(unfinished){
        step++;
        if(step >= this_path->way.size()){
            can_move = false;
    //        defeat();
            end();
        }
        else{
            block_now = map->all_block[this_path->way[step].y * this->parent->the_map->get_colomn() + this_path->way[step].x];
            animation->stop();
            animation1->stop();
            animation2->stop();
            animation3->stop();
            if(!block_now->empty()){
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
}

void Daida::attack(){       //一次攻击，攻击频率为 hz
    //游戏结束所有操作中断
    if(unfinished){
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
    this->step = step ? step : -1;
    this->resize(60, 90);
    this->map = map;
    x_now = this_path->way[step].x*70 + 70;
    y_now = this_path->way[step].y*70 - 30;
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
    //初始化animation
    animation = new QPropertyAnimation(this, "geometry");
    animation2 = new QPropertyAnimation(gif, "geometry");
}

void Skeleton::start_move(){
    move_once();
    //每隔3s进行一次递归
    QTimer::singleShot(3000, this, [=](){
        if(unfinished && can_move) start_move();
    });
}

void Skeleton::stop_move(){
    //游戏结束所有操作中断
    if(unfinished){
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
}

void Skeleton::move_once(){//3s完成线性的行走
    //游戏结束所有操作中断
    if(unfinished){
        movie = new QMovie(":/res/SkeletonWalk.gif");
        gif->setMovie(movie);
        movie->start();
        gif->show();
        step++;
        if(step >= this_path->way.size()){
            can_move = false;
    //        defeat();
            end();
        }
        else{
            block_now = map->all_block[this_path->way[step].y * this->parent->the_map->get_colomn() + this_path->way[step].x];
            animation->stop();
            animation2->stop();
            //设置起始位置
            animation->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
            animation2->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
            //设置终止位置
            if(!block_now->empty()){
                //设置时间间隔
                animation->setDuration(1000);
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
                animation->setDuration(1500);
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
}

void Skeleton::attack(){
    //游戏结束所有操作中断
    if(unfinished){
        Defender* target = this->block_now->defender_in()->back();
        target->health_decrease(150, 900);
        gif->resize(120, 100);
        gif->move(this->x()-50, this->y() - 10);
        movie = new QMovie(":/res/SkeletonAttack.gif");
        gif->setMovie(movie);
        movie->start();
        gif->show();
    }
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
    cut_off(gif, 1400);
    this->deleteLater();
}


//蝙蝠
Bat::Bat(QWidget *parent, int which_path, Map* map, int step){
    this->setParent(parent);
    this->parent = (MainWindow*)parent;
    this->this_path = &(*map->path)[which_path];
    this->step = step ? step : -1;
    this->resize(70, 100);
    this->map = map;
    x_now = this_path->way[step].x*70 + 70;
    y_now = this_path->way[step].y*70 - 30;
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
    //初始化animation
    animation = new QPropertyAnimation(this, "geometry");
    animation2 = new QPropertyAnimation(gif, "geometry");
}

void Bat::start_move(){
    move_once();
    //每隔1.5s进行一次递归
    QTimer::singleShot(1500, this, [=](){
        if(unfinished && can_move) start_move();
    });
}

void Bat::stop_move(){
    //游戏结束所有操作中断
    if(unfinished){
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
}

void Bat::move_once(){//3s完成线性的行走
    //游戏结束所有操作中断
    if(unfinished){
        movie = new QMovie(":/res/BatFly.gif");
        gif->setMovie(movie);
        movie->start();
        gif->show();
        step++;
        if(step >= this_path->way.size()){
            can_move = false;
    //        defeat();
            end();
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
                    stop_move();
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
        Defender* target = this->block_now->defender_in()->back();
        target->health_decrease(50, 300);
        movie = new QMovie(":/res/BatAttack.gif");
        gif->setMovie(movie);
        movie->start();
        gif->show();
    }
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
    cut_off(gif, 1100);
    this->deleteLater();
}

BlackWitch::BlackWitch(QWidget *parent, int which_path, Map* map, int step){
    this->setParent(parent);
    this->parent = (MainWindow*)parent;
    this->this_path = &(*map->path)[which_path];
    this->which_path = which_path;
    this->step = step ? step : -1;
    this->resize(70, 90);
    this->map = map;
    x_now = this_path->way[step].x*70 + 70;
    y_now = this_path->way[step].y*70 - 20;
    this->move(x_now, y_now);
    this->health = 400;
    //初始化gif
    gif = new QLabel;
    gif->setParent(parent);
    gif->resize(70, 70);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->setScaledContents(true);
    gif->hide();
    gif->move(x_now, y_now);
    //初始化anmiation
    animation = new QPropertyAnimation(this, "geometry");
    animation2 = new QPropertyAnimation(gif, "geometry");
}

void BlackWitch::start_move(){
    if(!started) start_call();
    move_once();
    //每隔4s进行一次递归，厌战且有召唤能力
    QTimer::singleShot(4000, this, [=](){
        if(unfinished && can_move) start_move();
    });
}

void BlackWitch::move_once(){
    //游戏结束所有操作中断
    if(unfinished){
        movie = new QMovie(":/res/BlackWitchFly.gif");
        gif->setMovie(movie);
        movie->start();
        gif->show();
        step++;
        if(step >= this_path->way.size()){
            can_move = false;
    //        defeat();
            end();
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
        started = true;
        QTimer::singleShot(7000, this, [=](){
            movie = new QMovie(":/res/BlackWitchCall.gif");
            gif->setMovie(movie);
            movie->start();
            gif->show();
            //0.3s后召唤蝙蝠
            QTimer::singleShot(300, this, [=](){
                this->parent->the_map->add_enemy(parent, which_path, Enemies::Bat, step);
            });
            QTimer::singleShot(500, this, [=](){
                movie = new QMovie(":/res/BlackWitchFly.gif");
                gif->setMovie(movie);
                movie->start();
                gif->show();
            });
            start_call();
        });
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
    movie = new QMovie(":/res/BlackWitchDead.gif");
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

void BlackWitch::stop_move(){

}

void BlackWitch::attack(){

}







