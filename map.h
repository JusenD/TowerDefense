#ifndef MAP_H
#define MAP_H

#include<QWidget>
#include"path.h"
#include<enemy.h>
#include"block.h"

using namespace std;
struct amap{
    int row = 0;
    int colomn = 0;
    int* array = nullptr;
    //地图初始资源和增长速度
    int original_source = 0;
    int source_speed = 0;
    int add_once = 0;
//    int array[10][15] = {
//        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
//    };
    vector<Single_Path> path;
};

class Map
{
    int source = 0;
    int ADD_ONCE = 50;
    int ADD_TIME = 4000;
    void increase_source();
    int row = 0;
    int colomn = 0;
public:
    static vector<amap> all;
    static void init();
    Map(QWidget* parent, int num);
    int get_colomn();
    int get_row();
    QWidget* parent;
    vector<Single_Path>* path;
    vector<Enemy*> all_enemy;
    vector<Block*> all_block;
    void add_enemy(QWidget *parent, int which_path, int who);
    void add_defender(Block* where, Defender* single_defender);
    int *array;
    //资源系统
    int get_source();
    void add_source(int num);
};

#endif // MAP_H
