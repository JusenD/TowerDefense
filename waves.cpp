#include "waves.h"
#include<QTime>

Waves::Waves(Map* the_map)
{
    this->the_map = the_map;
    QTime randtime = QTime::currentTime();
    srand(randtime.msec()+randtime.second()*1000); //初始化随机数
}

void Waves::start(int add_once, int time_interval, int max_level_enemy){
    clk = new QTimer(the_map->parent);
    this->add_once = add_once;
    clk_internal = new QTimer(the_map->parent);
    QObject::connect(clk, &QTimer::timeout, [=](){
        clk_internal->stop();
        now = 0;
        clk_internal->start(500);
    });
    QObject::connect(clk_internal, &QTimer::timeout, [=](){
        if(now < add_once){
            int random_enemy = rand() % max_level_enemy;
            int random_path = rand() % the_map->path->size();
            the_map->add_enemy(the_map->parent, random_path, random_enemy);
            now++;
        }
    });
    clk->start(time_interval);
}

void Waves::stategy(){
    start(1, 8000, 1);
    QTimer::singleShot(25000, [=](){
//        clk->stop();
//        clk_internal->stop();
        start(2, 8000, 1);
        QTimer::singleShot(25000, [=](){
//            clk->stop();
//            clk_internal->stop();
            start(3, 8000, 2);
            QTimer::singleShot(25000, [=](){
//                clk->stop();
//                clk_internal->stop();
                start(3, 8000, 3);
            });
            QTimer::singleShot(30000, [=](){
//                clk->stop();
//                clk_internal->stop();
                start(4, 10000, 4);
                NO_END = new QTimer();
                add_once = 5;
                QObject::connect(NO_END, &QTimer::timeout, [=](){
//                    clk->stop();
//                    clk_internal->stop();
                    start(add_once, 8000, 4);
                    add_once++;
                });
                NO_END->start(25000);
            });
        });
    });
}

void Waves::stop(){
    clk->stop();
    if(NO_END) NO_END->stop();
}

