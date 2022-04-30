#include "defender.h"
#include<mainwindow.h>
#include<QPainter>
#include"block.h"
#include"QDebug"
#include"enemy.h"
#include<QTimer>
#include<QMovie>

extern void cut_off(QLabel*, int num);
Defender::Defender(QWidget *parent)
    : QWidget{parent}
{
    place = NULL;
}

void Defender::add(Block* place){
    place = NULL;
    this->parent = parent;
}

int Defender::get_cost(){
    return cost;
}

void Defender::health_decrease(int n, int time){
    QTimer::singleShot(time, this, [=](){
        if(health>n) health-=n;
        else die();
    });
}

void Defender::die(){
    this->place->delete_defender(this);
    this->~Defender();
}

void Defender::stop(){
    this->unfinished = false;
}

bool Defender::is_fighter(){
    return fighter;
}

void Defender::mouseReleaseEvent(QMouseEvent *event){
    if(((MainWindow*)parent)->can_remove()){
        ((MainWindow*)parent)->change_remove(false);
        this->die();
        emit ((MainWindow*)parent)->get_selection()->mouseReleaseEvent(event);
    }
}

Boji::Boji(QWidget *parent)
    : Defender{parent}
{
//    this->place = place;
    this->parent = parent;
    this->resize(70, 70);
    this->health = 300;
    this->damage = 0;
    this->cost = 100;
    this->fighter = true;
//   this->move(place->x(), place->y());
}

int Boji::get_cost(){
    return this->cost;
}

void Boji::paintEvent(QPaintEvent *){
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/boji.png");
    painter.drawPixmap(0, 0, 70, 70, pix);
}

void Boji::add(Block* place){
    this->place = place;
    //设置自身位置(已经被添加到了所在Block的all_defender里
    this->move(place->x(), place->y());
    qDebug()<<place->x()<<place->y();
    this->show();
//    this->attack();
}

////攻击
//void Boji::attack(){
//    vector<Enemy*>* all_enemy = &((MainWindow*)(this->parent))->the_map->all_enemy;
//    bool whether = false;
//    //范围伤害
//    for(int i = 0; i < all_enemy->size(); i++){
//        if((*all_enemy)[i]->block_now == this->place){
//            if((*all_enemy)[i]->is_ground()){
//                (*all_enemy)[i]->health_decrease(damage, 260);
//                whether = true;
//            }
//        }
//    }
//    //设置攻击频率 为2s一次
//    if(whether){
//        //执行攻击动画
//        QPropertyAnimation *animation1 = new QPropertyAnimation(this, "geometry");
//        //设置时间间隔
//        animation1->setDuration(250);
//        //设置起始位置
//        animation1->setStartValue(QRect(place->x(), place->y(), this->width(), this->height()));
//        //设置结束位置
//        animation1->setEndValue(QRect(place->x() + 4, place->y() - 7, this->width(), this->height()));
//        animation1->setEasingCurve(QEasingCurve::InOutCubic);
//        animation1->start();
//        QTimer::singleShot(250, this, [=](){
//            QPropertyAnimation *animation2 = new QPropertyAnimation(this, "geometry");
//            //设置时间间隔
//            animation2->setDuration(150);
//            //设置起始位置
//            animation2->setStartValue(QRect(place->x() + 4, place->y() - 7, this->width(), this->height()));
//            //设置结束位置
//            animation2->setEndValue(QRect(place->x() + 7, place->y(), this->width(), this->height()));
//            animation2->setEasingCurve(QEasingCurve::InOutCubic);
//            animation2->start();
//            QTimer::singleShot(150, this, [=](){
//                QPropertyAnimation *animation3 = new QPropertyAnimation(this, "geometry");
//                //设置时间间隔
//                animation3->setDuration(150);
//                //设置起始位置
//                animation3->setStartValue(QRect(place->x() + 7, place->y(), this->width(), this->height()));
//                //设置结束位置
//                animation3->setEndValue(QRect(place->x(), place->y(), this->width(), this->height()));
//                animation3->setEasingCurve(QEasingCurve::InOutCubic);
//                animation3->start();
//            });
//        });
//    }
//    //递归调用
//    QTimer::singleShot(2000, this, [=](){
//        attack();
//    });
//}

Witch::Witch(QWidget *parent)
    : Defender{parent}
{
//    this->place = place;
    this->parent = parent;
    this->resize(70, 100);
    this->health = 100;
    this->damage = 100;
    this->cost = 200;
    this->range = 300;
    this->fighter = false;
//   this->move(place->x(), place->y());

}

int Witch::get_cost(){
    return this->cost;
}

void Witch::add(Block* place){
    this->place = place;
    //设置自身位置(已经被添加到了所在Block的all_defender里
    this->move(place->x(), place->y());
//    qDebug()<<place->x()<<place->y();
    this->show();
    //设置事件过滤器
    this->installEventFilter(this);
    //显示等待动画gif
    gif = new QLabel(parent);
    gif->setFixedSize(70, 100);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->setScaledContents(true);
    gif->move(place->x(), place->y() - 30);
    QMovie* movie = new QMovie(":/res/wait.gif");
    gif->setMovie(movie);
    movie->start();
    gif->show();
    //初始化range_circle
    range_circle = new QLabel(parent);
    range_circle->resize(0, 0);
    QPixmap pix;
    pix.load(":/res/range_circle.png");
    pix.scaled(range, range);
    range_circle->setScaledContents(true);
    range_circle->setPixmap(pix);
    range_circle->setAttribute(Qt::WA_TransparentForMouseEvents);
    //初始化魔法攻击
    this->bang = new QLabel(parent);
    bang->hide();
    bang->move(this->x() + 35, this->y()-10);
    bang->setFixedSize(40, 40);
    bang->setScaledContents(true);
    QMovie* bang_movie = new QMovie(":/res/bang.gif");
    bang->setMovie(bang_movie);
    bang_movie->start();
    this->attack();
}

void Witch::attack(){
    if(unfinished){
        vector<Enemy*>* all_enemy = &((MainWindow*)(this->parent))->the_map->all_enemy;
        bool whether = false;
        int target = 0;
        //范围判定,只能攻击一个
        for(int i = 0; i < all_enemy->size(); i++){
            int distance = ((*all_enemy)[i]->x() - this->place->x())*((*all_enemy)[i]->x() - this->place->x())
                    + ((*all_enemy)[i]->y() - this->place->y())*((*all_enemy)[i]->y() - this->place->y());
            if(distance <= range*range){
                (*all_enemy)[i]->health_decrease(damage, 1100);
                whether = true;
                target = i;
                break;
            }
        }
        if(whether){
            //执行攻击动画
            QMovie* movie = new QMovie(":/res/attack.gif");
            gif->setMovie(movie);
            movie->start();
            gif->show();
            QTimer::singleShot(300, this, [=](){
                this->enemy_x = (*all_enemy)[target]->x();
                this->enemy_y = (*all_enemy)[target]->y();
                bang->show();
                QPropertyAnimation *animation = new QPropertyAnimation(bang, "geometry");
                //设置时间间隔
                animation->setDuration(800);
                //设置起始位置
                animation->setStartValue(QRect(bang->x(), bang->y(), bang->width(), bang->height()));
                //设置结束位置
                animation->setEndValue(QRect(enemy_x + 15, enemy_y + 25, bang->width(), bang->height()));
                animation->setEasingCurve(QEasingCurve::InCubic);
                animation->start();
                QTimer::singleShot(850, this, [=](){
                    bang->hide();
                    bang->move(place->x()+35, place->y()-10);
                });
            });
            QTimer::singleShot(700, this, [=](){
                QMovie* movie = new QMovie(":/res/wait.gif");
                gif->setMovie(movie);
                movie->start();
            });
        }
        //递归调用
        QTimer::singleShot(3000, this, [=](){
            attack();
        });
    }
}

//用事件过滤器点击女巫后显示攻击范围
bool Witch::eventFilter(QObject *obj, QEvent *event){
    if(obj == this){
        if(event->type() == QEvent::MouseButtonRelease  && !((MainWindow*)parent)->can_remove()){
            if(!bounce_out){
                //创建circle_range label
                range_circle = new QLabel(parent);
                range_circle->resize(0, 0);
                QPixmap pix;
                pix.load(":/res/range_circle.png");
                pix.scaled(range, range);
                range_circle->setScaledContents(true);
                range_circle->setPixmap(pix);
                range_circle->setAttribute(Qt::WA_TransparentForMouseEvents);
                animation1 = new QPropertyAnimation(range_circle);
                animation2 = new QPropertyAnimation(range_circle);
                /* 设置动画持续时长 */
                animation1->setDuration(200);
                /* 设置动画目标 */
                animation1->setTargetObject(range_circle);
                /* 设置窗口几何属性(位置，大小)作为动画参考 */
                animation1->setPropertyName("geometry");
                /* 设置动画开始坐标和大小(QRect) */
                /* 居中显示数值计算：160 => (320/2 - 100); 120 => (240/2 - 100) */
                animation1->setStartValue(QRect(this->x() + 35, this->y() + 35, 0, 0));
                /* 设置动画结束坐标和大小(QRect) */
                /* 居中显示数值计算(差值是基于开始动画的100/2)：160 => (160 - 100/2); 70 => (120 - 100/2) */
                animation1->setEndValue(QRect(this->x() + 35 - range, this->y() + 35 - range, 2*range, 2*range));
                /* 开始动画 */
                range_circle->show();
                animation1->start();
                bounce_out = true;
            }
        else{
            animation1->stop();
            /* 设置动画持续时长 */
            animation2->setDuration(150);
            /* 设置动画目标 */
            animation2->setTargetObject(range_circle);
            /* 设置窗口几何属性(位置，大小)作为动画参考 */
            animation2->setPropertyName("geometry");
            /* 设置动画开始坐标和大小(QRect) */
            animation2->setStartValue(QRect(this->x() + 35 - range, this->y() + 35 - range, 2*range, 2*range));
            //animation->setStartValue(QRect(this->x() + 35, this->y() + 35, 0, 0));
            /* 设置动画结束坐标和大小(QRect) */
            animation2->setEndValue(QRect(this->x() + 35, this->y() + 35, 0, 0));
            /* 开始动画 */
            animation2->start();
            cut_off(range_circle, 250);
            bounce_out = false;
        }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void Witch::die(){
    QMovie* movie = new QMovie(":/res/die.gif");
    gif->setMovie(movie);
    movie->start();
    this->place->delete_defender(this);
    cut_off(gif, 1000);
    cut_off(bang, 1000);
    if(bounce_out){
        //如果死亡时有范围圈，则将范围圈弹回
        animation1->stop();
        animation2->setDuration(150);
        animation2->setTargetObject(range_circle);
        animation2->setPropertyName("geometry");
        animation2->setStartValue(QRect(this->x() + 35 - range, this->y() + 35 - range, 2*range, 2*range));
        animation2->setEndValue(QRect(this->x() + 35, this->y() + 35, 0, 0));
        animation2->start();
        cut_off(range_circle, 250);
        bounce_out = false;
    }
    this->~Witch();
}

EvilWizard::EvilWizard(QWidget *parent)
    : Defender{parent}
{
//    this->place = place;
    this->parent = parent;
    this->resize(70, 70);
    this->health = 200;
    this->damage = 100;
    this->cost = 300;
    this->fighter = true;
    //初始化gif Label
    gif = new QLabel;
    gif->setParent(parent);
    gif->setFixedSize(300, 300);
    gif->setScaledContents(true);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    movie = new QMovie(":/res/EvilWizardWait.gif");
    movie->start();
    gif->setMovie(movie);
    gif->hide();

//   this->move(place->x(), place->y());
}

int EvilWizard::get_cost(){
    return this->cost;
}

void EvilWizard::add(Block* place){
    this->place = place;
    //设置自身位置(已经被添加到了所在Block的all_defender里
    this->move(place->x(), place->y());
    qDebug()<<place->x()<<place->y();
    this->show();
    gif->move(place->x() - 120, place->y() - 140);
    gif->show();
    this->attack();
}

//攻击
void EvilWizard::attack(){
    if(unfinished){
        vector<Enemy*>* all_enemy = &((MainWindow*)(this->parent))->the_map->all_enemy;
        bool whether = false;
        //范围判定 可以攻击前方两格内的所有敌人
        int range = 105;
        for(int i = 0; i < all_enemy->size(); i++){
            int distance = (*all_enemy)[i]->block_now->x() - this->place->x();
            if(distance>0 && distance <= range && (*all_enemy)[i]->block_now->y() - this->place->y() < 20
                    && (*all_enemy)[i]->block_now->y() - this->place->y() > -20){
                (*all_enemy)[i]->health_decrease(damage, 300);
                whether = true;
            }
        }
        //范围伤害
        for(int i = 0; i < all_enemy->size(); i++){
            if((*all_enemy)[i]->block_now == this->place){
                (*all_enemy)[i]->health_decrease(damage, 500);
                whether = true;
            }
        }
        //设置攻击频率 为3s一次
        if(whether){
            //执行攻击动画
            movie = new QMovie(":/res/EvilWizardAttack.gif");
            movie->start();
            gif->setMovie(movie);
            gif->show();
            QTimer::singleShot(1000, this, [=](){
                movie = new QMovie(":/res/EvilWizardWait.gif");
                movie->start();
                gif->setMovie(movie);
                gif->show();
            });
        }
        //递归调用
        QTimer::singleShot(3000, this, [=](){
            attack();
        });
    }
}

void EvilWizard::die(){
    QMovie* movie = new QMovie(":/res/EvilWizardDead.gif");
    gif->setMovie(movie);
    movie->start();
    this->place->delete_defender(this);
    cut_off(gif, 800);
    this->~EvilWizard();
}

Droid::Droid(QWidget *parent)
    : Defender{parent}
{
//    this->place = place;
    this->parent = parent;
    this->resize(70, 70);
    this->health = 100;
    this->damage = 0;
    this->cost = 100;
    this->fighter = true;
//   this->move(place->x(), place->y());
}

int Droid::get_cost(){
    return this->cost;
}

void Droid::add(Block* place){
    this->place = place;
    //设置自身位置(已经被添加到了所在Block的all_defender里
    this->move(place->x(), place->y());
//    qDebug()<<place->x()<<place->y();
    this->show();
    //显示等待动画gif
    gif = new QLabel(parent);
    gif->setFixedSize(140, 90);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->setScaledContents(true);
    gif->move(place->x()-35, place->y() - 25);
    QMovie* movie = new QMovie(":/res/DroidWait.gif");
    gif->setMovie(movie);
    movie->start();
    gif->show();
    this->attack();
}

void Droid::attack(){
    if(unfinished){
        //执行充能动画
        QMovie* movie = new QMovie(":/res/DroidAttack.gif");
        gif->setMovie(movie);
        movie->start();
        gif->show();
        QTimer::singleShot(1100, this, [=](){
            QMovie* movie = new QMovie(":/res/DroidWait.gif");
            gif->setMovie(movie);
            movie->start();
            ((MainWindow*)parent)->the_map->add_source(50);
        });
        //递归调用
        QTimer::singleShot(10000, this, [=](){
            attack();
        });
    }
}

void Droid::die(){
    QMovie* movie = new QMovie(":/res/DroidDead.gif");
    gif->setMovie(movie);
    movie->start();
    this->place->delete_defender(this);
    cut_off(gif, 700);
    this->~Droid();
}

Soildier::Soildier(QWidget *parent)
    : Defender (parent)
{
    this->parent = parent;
    this->resize(70, 70);
    this->health = 150;
    this->damage = 10;
    this->cost = 0;
    this->fighter = true;
}

int Soildier::get_cost(){
    return this->cost;
}

void Soildier::add(Block* place, int x_now, int y_now){
    this->place = place;
    //装载走路动画
    gif = new QLabel(parent);
    gif->setFixedSize(100, 70);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->setScaledContents(true);
    gif->move(x_now, y_now);
    movie = new QMovie(":/res/SoildierWalk.gif");
    gif->setMovie(movie);
    movie->start();
    gif->show();
    //设置移动动画
    animation1 = new QPropertyAnimation(this, "geometry");
    animation2 = new QPropertyAnimation(gif, "geometry");
    animation1->setStartValue(QRect(x_now, y_now, this->width(), this->height()));
    animation2->setStartValue(QRect(place->x(), place->y(), 100, 70));
    animation1->setDuration(1000);
    animation2->setDuration(1000);
    animation1->start();
    animation2->start();
//    this->move(place->x(), place->y());
    this->show();
    QTimer::singleShot(1100, this, [=](){
        this->attack();
    });
}

void Soildier::attack(){
    if(unfinished){
        //显示等待动画
        movie = new QMovie(":/res/SoildierWait.gif");
        gif->setMovie(movie);
        movie->start();
        gif->show();
        vector<Enemy*>* all_enemy = &((MainWindow*)(this->parent))->the_map->all_enemy;
            bool whether = false;
            //只能攻击一个
            for(int i = 0; i < all_enemy->size(); i++){
                if((*all_enemy)[i]->block_now == this->place){
                    if((*all_enemy)[i]->is_ground()){
                        (*all_enemy)[i]->health_decrease(damage, 260);
                        whether = true;
                        break;
                    }
                }
            }
    }
}

void Soildier::die(){

}







