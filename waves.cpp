#include "waves.h"
#include<QTime>

Waves::Waves(Map* the_map)
{
    this->the_map = the_map;
    QTime randtime = QTime::currentTime();
    srand(randtime.msec()+randtime.second()*1000); //初始化随机数
    clk = new QTimer();
    clk_internal = new QTimer();
    QObject::connect(clk_internal, &QTimer::timeout, [=](){
        if(now < add_once){
            int random_enemy = rand() % max_level_enemy;
            int random_path = rand() % the_map->path->size();
            the_map->add_enemy(the_map->parent, random_path, random_enemy);
            now++;
        }
    });
    QObject::connect(clk, &QTimer::timeout, [=](){
        clk_internal->stop();
        now = 0;
        clk_internal->start(1000);
    });
}

void Waves::start(int add_once, int time_interval, int max_level_enemy){
    this->add_once = add_once;
    this->max_level_enemy = max_level_enemy;
    clk->start(time_interval);
}

void Waves::stategy(){
    start(1, 10000, 1);
    QTimer::singleShot(25000, this->clk, [=](){
        start(2, 10000, 1);
        QTimer::singleShot(25000, this->clk, [=](){
            start(3, 10000, 2);
            QTimer::singleShot(25000, this->clk, [=](){
                start(3, 10000, 3);
            });
            QTimer::singleShot(30000, this->clk, [=](){

                start(4, 10000, 4);
                NO_END = new QTimer();
                add_once = 5;
                QObject::connect(NO_END, &QTimer::timeout, [=](){
                    start(add_once, 10000, 4);
                    add_once++;
                });
                NO_END->start(25000);
            });
        });
    });
    QTimer::singleShot(50000, this->clk, [=](){
        the_map->add_enemy(the_map->parent, rand()%the_map->path->size(), Enemy::Plus);
        the_map->add_enemy(the_map->parent, rand()%the_map->path->size(), Enemy::Plus);
    });
    QTimer::singleShot(100000, this->clk, [=](){
        the_map->add_enemy(the_map->parent, rand()%the_map->path->size(), Enemy::Bot);
        the_map->add_enemy(the_map->parent, rand()%the_map->path->size(), Enemy::Plus);
    });
    QTimer::singleShot(165000, this->clk, [=](){
        the_map->add_enemy(the_map->parent, rand()%the_map->path->size(), Enemy::Plus);
        the_map->add_enemy(the_map->parent, rand()%the_map->path->size(), Enemy::Bot);
        QTimer::singleShot(1000, this->clk, [=](){
            the_map->add_enemy(the_map->parent, rand()%the_map->path->size(), Enemy::Bot);
            QTimer::singleShot(1000, this->clk, [=](){
                the_map->add_enemy(the_map->parent, rand()%the_map->path->size(), Enemy::Plus);
                the_map->add_enemy(the_map->parent, rand()%the_map->path->size(), Enemy::Bot);
            });
        });
    });
    QTimer::singleShot(220000, this->clk, [=](){
        the_map->victory();
        this->stop();
    });
}

void Waves::stop(){
    if(!has_stoped){
        has_stoped = true;
        clk->disconnect();
        clk_internal->disconnect();
        clk->stop();
        clk_internal->stop();
        if(NO_END) {
            NO_END->disconnect();
            NO_END->stop();
            NO_END->deleteLater();
        }
        clk->deleteLater();
        clk_internal->deleteLater();
    }
}

