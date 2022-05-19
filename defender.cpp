#include "defender.h"
#include<mainwindow.h>
#include<QPainter>
#include"block.h"
#include"QDebug"
#include"enemy.h"
#include<QTimer>
#include<QMovie>
#include"healthbar.h"

bool Defender::on_delete = false;
void Defender::cut_off(QLabel* gif, int time){
    QTimer::singleShot(time, [=](){
        if(!on_delete && gif) gif->deleteLater();
    });
}

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

int Defender::get_health(){
    return this->health;
}

int Defender::get_original_health(){
    return this->original_health;
}

QLabel* Defender::get_gif(){
    return this->gif;
}

void Defender::health_decrease(int n, int time){
    QTimer::singleShot(time, this, [=](){
        if(health>n) health-=n;
        else die();
    });
}

void Defender::die(){
    attack_clk->stop();
    this->place->delete_defender(this);
    this->health_bar->deleteLater();
    this->deleteLater();
}

void Defender::stop(){
    this->unfinished = false;
}

bool Defender::is_fighter(){
    return fighter;
}

bool Defender::is_tower(){
    return tower;
}

bool Defender::is_king(){
    return king;
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
    this->original_health = health;
    this->damage = 0;
    this->cost = 100;
    this->fighter = true;
//   this->move(place->x(), place->y());
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
    this->gif = new QLabel(parent);
    gif->setFixedSize(70, 100);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->setScaledContents(true);
    gif->move(place->x(), place->y() - 30);
    gif->show();
    //初始化血条
    this->health_bar = new healthBar(this);
    health_bar->show();
    health_bar->move(5, 8);
    qDebug()<<place->x()<<place->y();
    this->show();
    attack_clk = new QTimer(this->gif);
    connect(attack_clk, &QTimer::timeout, this->gif, [=](){
        this->gif->update();
    });
    attack_clk->start(100);
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
    this->resize(70, 70);
    this->health = 100;
    this->original_health = health;
    this->damage = 100;
    this->cost = 200;
    this->range = 300;
    this->fighter = false;
//   this->move(place->x(), place->y());
    //初始化animation
    animation1 = new QPropertyAnimation(range_circle);
    animation2 = new QPropertyAnimation(range_circle);
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
    movie = new QMovie(":/res/wait.gif");
    gif->setMovie(movie);
    movie->start();
    gif->show();
    //初始化血条
    this->health_bar = new healthBar(this);
    health_bar->show();
    health_bar->move(5, 8);
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
    bang_movie = new QMovie(":/res/bang.gif");
    bang->setMovie(bang_movie);
    bang_movie->start();
    //初始化时钟
    attack_clk = new QTimer(this);
    //每隔3s攻击一次
    attack_clk->setInterval(3000);
    connect(attack_clk, &QTimer::timeout, this, &Witch::attack);
    this->attack();
    attack_clk->start();
}

void Witch::attack(){
    if(unfinished){
        vector<Enemy*>* all_enemy = &((MainWindow*)(this->parent))->the_map->all_enemy;
        bool whether = false;
        Enemy* target = 0;
        //范围判定,只能攻击一个
        for(int i = 0; i < all_enemy->size(); i++){
            if(!(*all_enemy)[i]) continue;
            int distance = ((*all_enemy)[i]->x() - this->place->x())*((*all_enemy)[i]->x() - this->place->x())
                    + ((*all_enemy)[i]->y() - this->place->y())*((*all_enemy)[i]->y() - this->place->y());
            if(distance <= range*range){
                (*all_enemy)[i]->health_decrease(damage, 1100);
                whether = true;
                target = (*all_enemy)[i];
                this->enemy_x = (*all_enemy)[i]->x();
                this->enemy_y = (*all_enemy)[i]->y();
                break;
            }
        }
        if(whether){
            //执行攻击动画
            QMovie* former = movie;
            movie = new QMovie(":/res/attack.gif");
            former->deleteLater();
            gif->setMovie(movie);
            movie->start();
            gif->show();
            QTimer::singleShot(300, this, [=](){
                if(target){
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
                }
            });
            QTimer::singleShot(700, this, [=](){
                QMovie* former = movie;
                movie = new QMovie(":/res/wait.gif");
                former->deleteLater();
                gif->setMovie(movie);
                movie->start();
            });
        }
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
                animation1->stop();
                animation2->stop();
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
    attack_clk->stop();
    QMovie* former = movie;
    movie = new QMovie(":/res/die.gif");
    former->deleteLater();
    gif->setMovie(movie);
    movie->start();
    this->health_bar->deleteLater();
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
    this->deleteLater();
}

EvilWizard::EvilWizard(QWidget *parent)
    : Defender{parent}
{
//    this->place = place;
    this->parent = parent;
    this->resize(70, 70);
    this->health = 200;
    this->original_health = health;
    this->damage = 100;
    this->cost = 300;
    this->fighter = true;
    //初始化gif Label
    gif = new QLabel;
    gif->setParent(parent);
    gif->setFixedSize(200, 170);
    gif->setScaledContents(true);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    movie = new QMovie(":/res/EvilWizardWait.gif");
    movie->start();
    gif->setMovie(movie);
    gif->hide();
//   this->move(place->x(), place->y());
}

QLabel* EvilWizard::get_gif(){
    return this->giff;
}

void EvilWizard::add(Block* place){
    this->place = place;
    //设置自身位置(已经被添加到了所在Block的all_defender里
    this->move(place->x(), place->y());
    this->show();
    gif->move(place->x() - 30, place->y() - 95);
    gif->show();
    //初始化血条
    giff = new QLabel;
    giff->setParent(parent);
    giff->setFixedSize(100, 100);
    giff->move(this->x(), this->y()-30);
    giff->setAttribute(Qt::WA_TransparentForMouseEvents);
    giff->show();
    this->health_bar = new healthBar(this);
    health_bar->show();
    health_bar->move(2, 8);
    //初始化时钟
    attack_clk = new QTimer(this);
    //每隔3s攻击一次
    attack_clk->setInterval(3000);
    connect(attack_clk, &QTimer::timeout, this, &EvilWizard::attack);
    this->attack();
    attack_clk->start();
}

//攻击
void EvilWizard::attack(){
    if(unfinished){
        vector<Enemy*>* all_enemy = &((MainWindow*)(this->parent))->the_map->all_enemy;
        bool whether = false;
        //范围判定 可以攻击前方两格内的所有敌人
        int range = 105;
        for(int i = 0; i < all_enemy->size(); i++){
            if(!(*all_enemy)[i]) continue;
            int distance = (*all_enemy)[i]->block_now->x() - this->place->x();
            if(distance>0 && distance <= range && (*all_enemy)[i]->block_now->y() - this->place->y() < 20
                    && (*all_enemy)[i]->block_now->y() - this->place->y() > -20){
                (*all_enemy)[i]->health_decrease(damage, 300);
                whether = true;
            }
        }
        //范围伤害
        for(int i = 0; i < all_enemy->size(); i++){
            if(!(*all_enemy)[i]) continue;
            if((*all_enemy)[i]->block_now == this->place){
                (*all_enemy)[i]->health_decrease(damage, 300);
                whether = true;
            }
        }
        //设置攻击频率 为3s一次
        if(whether){
            //执行攻击动画
            QMovie* former = movie;
            movie = new QMovie(":/res/EvilWizardAttack.gif");
            former->deleteLater();
            movie->start();
            gif->setMovie(movie);
            gif->show();
            QTimer::singleShot(1000, this, [=](){
                QMovie* former = movie;
                movie = new QMovie(":/res/EvilWizardWait.gif");
                former->deleteLater();
                movie->start();
                gif->setMovie(movie);
                gif->show();
            });
        }
    }
}

void EvilWizard::die(){
    attack_clk->stop();
    QMovie* former = movie;
    movie = new QMovie(":/res/EvilWizardDead.gif");
    former->deleteLater();
    gif->setMovie(movie);
    movie->start();
    this->health_bar->deleteLater();
    this->place->delete_defender(this);
    cut_off(gif, 800);
    this->deleteLater();
}

Droid::Droid(QWidget *parent)
    : Defender{parent}
{
//    this->place = place;
    this->parent = parent;
    this->resize(70, 70);
    this->health = 100;
    this->original_health = health;
    this->damage = 0;
    this->cost = 100;
    this->fighter = true;
//   this->move(place->x(), place->y());
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
    movie = new QMovie(":/res/DroidWait.gif");
    gif->setMovie(movie);
    movie->start();
    gif->show();
    //初始化血条
    this->health_bar = new healthBar(this);
    health_bar->show();
    health_bar->move(23, 8);
    //初始化时钟
    attack_clk = new QTimer(this);
    //每隔10s攻击一次
    attack_clk->setInterval(10000);
    connect(attack_clk, &QTimer::timeout, this, &Droid::attack);
    attack_clk->start();
}

void Droid::attack(){
    if(unfinished){
        //执行充能动画
        QMovie* former = movie;
        movie = new QMovie(":/res/DroidAttack.gif");
        former->deleteLater();
        gif->setMovie(movie);
        movie->start();
        gif->show();
        QTimer::singleShot(1100, this, [=](){
            QMovie* former = movie;
            movie = new QMovie(":/res/DroidWait.gif");
            former->deleteLater();
            gif->setMovie(movie);
            movie->start();
            ((MainWindow*)parent)->the_map->add_source(50);
        });
    }
}

void Droid::die(){
    attack_clk->stop();
    QMovie* former = movie;
    movie = new QMovie(":/res/DroidDead.gif");
    former->deleteLater();
    gif->setMovie(movie);
    movie->start();
    this->health_bar->deleteLater();
    this->place->delete_defender(this);
    cut_off(gif, 700);
    this->deleteLater();
}

Soildier::Soildier(QWidget *parent)
    : Defender (parent)
{
    this->parent = parent;
    this->resize(70, 70);
    this->health = 300;
    this->original_health = health;
    this->damage = 30;
    this->cost = 0;
    this->fighter = true;
}


void Soildier::add(Block* place, int x_now, int y_now){
    this->place = place;
    //装载走路动画
    gif = new QLabel(parent);
    gif->setFixedSize(120, 84);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->setScaledContents(true);
    gif->move(x_now, y_now - 20);
    movie = new QMovie(":/res/SoildierWalk.gif");
    gif->setMovie(movie);
    movie->start();
    gif->show();
    //初始化血条
    this->health_bar = new healthBar(this);
    health_bar->show();
    health_bar->move(5, 8);
    //设置移动动画
    animation1 = new QPropertyAnimation(this, "geometry");
    animation2 = new QPropertyAnimation(gif, "geometry");
    animation1->setStartValue(QRect(x_now, y_now - 20, this->width(), this->height()));
    animation2->setEndValue(QRect(place->x(), place->y() - 20, 100, 70));
    animation1->setDuration(2000);
    animation2->setDuration(2000);
    animation1->start();
    animation2->start();
//    this->move(place->x(), place->y());
    this->show();
    QTimer::singleShot(2100, this, [=](){
        //显示等待动画
        QMovie* former = movie;
        movie = new QMovie(":/res/SoildierWait.gif");
        former->deleteLater();
        gif->setMovie(movie);
        movie->start();
        gif->show();
        //初始化时钟
        attack_clk = new QTimer(this);
        //每隔6s攻击一次
        attack_clk->setInterval(6000);
        connect(attack_clk, &QTimer::timeout, this, &Soildier::attack);
        this->attack();
        attack_clk->start();
    });
}

void Soildier::attack(){
    if(unfinished){
        bool whether = false;
        //只能攻击一个
        for(int i = 0; i < (&((MainWindow*)(this->parent))->the_map->all_enemy)->size(); i++){
            if((*(&((MainWindow*)(this->parent))->the_map->all_enemy))[i]&&(*(&((MainWindow*)(this->parent))->the_map->all_enemy))[i]->block_now == this->place){
                if((*(&((MainWindow*)(this->parent))->the_map->all_enemy))[i]&&(*(&((MainWindow*)(this->parent))->the_map->all_enemy))[i]->is_ground()){
                    (*(&((MainWindow*)(this->parent))->the_map->all_enemy))[i]->health_decrease(damage, 600);
                    (*(&((MainWindow*)(this->parent))->the_map->all_enemy))[i]->health_decrease(damage, 1400);
                    (*(&((MainWindow*)(this->parent))->the_map->all_enemy))[i]->health_decrease(damage, 2400);
                    whether = true;
                    break;
                }
            }
        }
        if(whether){
            QMovie* former = movie;
            movie = new QMovie(":/res/SoildierAttack.gif");
            former->deleteLater();
            gif->setMovie(movie);
            movie->start();
            gif->show();
            QTimer::singleShot(3000, this, [=](){
                //显示等待动画
                QMovie* former = movie;
                movie = new QMovie(":/res/SoildierWait.gif");
                former->deleteLater();
                gif->setMovie(movie);
                movie->start();
                gif->show();
            });
        }
    }
}

void Soildier::die(){
    attack_clk->stop();
    QMovie* former = movie;
    movie = new QMovie(":/res/SoildierDead.gif");
    former->deleteLater();
    gif->setMovie(movie);
    movie->start();
    this->health_bar->deleteLater();
    this->place->delete_defender(this);
    cut_off(gif, 3700);
    this->deleteLater();
}

King::King(QWidget *parent)
    : Defender{parent}
{
//    this->place = place;
    this->parent = parent;
    this->resize(70, 70);
    this->health = 400;
    this->original_health = health;
    this->damage = 0;
    this->cost = 800;
    this->fighter = false;
    this->king = true;
//   this->move(place->x(), place->y());
}

void King::add(Block* place){
    this->place = place;
    //设置自身位置(已经被添加到了所在Block的all_defender里
    this->move(place->x(), place->y());
//    qDebug()<<place->x()<<place->y();
    this->show();
    //显示等待动画gif
    gif = new QLabel(parent);
    gif->setFixedSize(100, 80);
    gif->setAttribute(Qt::WA_TransparentForMouseEvents);
    gif->setScaledContents(true);
    gif->move(place->x()-15, place->y()-10);
    movie = new QMovie(":/res/KingWait.gif");
    gif->setMovie(movie);
    movie->start();
    gif->show();
    //初始化血条
    this->health_bar = new healthBar(this);
    health_bar->show();
    health_bar->move(12, 0);
}

void King::setTarget(Block* target){
    this->target = target;
    //初始化时钟
    attack_clk = new QTimer(this);
    //每隔15s攻击一次
    attack_clk->setInterval(15000);
    connect(attack_clk, &QTimer::timeout, this, &King::attack);
    this->attack();
    attack_clk->start();
}

void King::attack(){
    if(unfinished){
        if(target && target->empty()){
            //执行召唤动画
            QMovie* former = movie;
            movie = new QMovie(":/res/KingCall.gif");
            former->deleteLater();
            gif->setMovie(movie);
            movie->start();
            gif->show();
            QTimer::singleShot(2500, this, [=](){
                //召唤皇家卫兵
                if(target->empty()){
                    class::Soildier* a_soildier = new class::Soildier(parent);
                    target->push_a_defender(a_soildier);
                    a_soildier->move(target->x(), target->y());
                    a_soildier->add(target, this->x(), this->y());
                    ((MainWindow*)parent)->the_map->all_defender.push_back(a_soildier);
                }
            });
            QTimer::singleShot(3000, this, [=](){
                QMovie* former = movie;
                movie = new QMovie(":/res/KingWait.gif");
                former->deleteLater();
                gif->setMovie(movie);
                movie->start();
            });
        }
    }
}

void King::die(){
    attack_clk->stop();
    QMovie* former = movie;
    movie = new QMovie(":/res/KingDead.gif");
    former->deleteLater();
    gif->setMovie(movie);
    movie->start();
    this->health_bar->deleteLater();
    this->place->delete_defender(this);
    cut_off(gif, 2500);
    this->deleteLater();
}







