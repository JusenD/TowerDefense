#ifndef WAVES_H
#define WAVES_H
#include"map.h"
#include<QTimer>

class Waves
{
    int add_once = 0;
    int time_interval = 10000;
    int max_level_enemy = 0;
    int now = 0;
    Map* the_map = nullptr;
    QTimer* clk = nullptr;
    QTimer* clk_internal = nullptr;
    QTimer* NO_END = nullptr;
public:
    Waves(Map* the_map);
    void start(int add_once, int time_interval, int max_level_enemy);
    void stategy();
    void stop();
};

#endif // WAVES_H
