#ifndef ENEMY_H
#define ENEMY_H

#include <QWidget>
#include"path.h"
#include"block.h"
#include<QPropertyAnimation>

class MainWindow;
class Map;
class Enemy : public QWidget
{
    Q_OBJECT
public:
    //枚举所有敌人种类
    enum Enemies{Daida, Skeleton, Bat, BlackWitch};
    //当前位置
    int x_now;
    int y_now;
    Block* block_now;
    //传入地图
    Map* map;
    //-1表示未进场
    int step = -1;
    virtual void start_move() = 0;
    virtual void stop_move() = 0;
    virtual void move_once() = 0;
    virtual void attack() = 0;
    void health_decrease(int n, int time);
    virtual void die() = 0;
    bool is_ground();
    bool is_fly();
    int get_x();
    int get_y();

protected:
    //设置生命值
    int health = 0;
    int damage = 0;
    bool can_move = true;
    bool ground = false;
    bool fly = false;
    bool dead = false;
    MainWindow* parent = nullptr;
    Single_Path* this_path = nullptr;
signals:
    void defeat();
};

class Ground_Enemy:public Enemy{
public:
    explicit Ground_Enemy()
    {
        ground = true;
    }
};

class Fly_Enemy:public Enemy{
public:
    explicit Fly_Enemy()
    {
        fly = true;
    }
};

class Daida:public Ground_Enemy{
public:
    explicit Daida(QWidget *parent, int which_path, Map* map, int step = 0);
    void paintEvent(QPaintEvent*);
    void start_move();
    void stop_move();
    void move_once();
    void attack();
    void die();
};

class Skeleton: public Ground_Enemy{
public:
    explicit Skeleton(QWidget *parent, int which_path, Map* map, int step = 0);
    void start_move();
    void stop_move();
    void move_once();
    void attack();
    void die();
    QLabel* gif = nullptr;
    QMovie* movie = nullptr;
    QPropertyAnimation* animation = nullptr;
    QPropertyAnimation* animation2 = nullptr;
};

class Bat: public Fly_Enemy{
public:
    explicit Bat(QWidget *parent, int which_path, Map* map, int step = 0);
    void start_move();
    void stop_move();
    void move_once();
    void attack();
    void die();
    QLabel* gif = nullptr;
    QMovie* movie = nullptr;
    QPropertyAnimation* animation = nullptr;
    QPropertyAnimation* animation2 = nullptr;
};

class BlackWitch : public Fly_Enemy{
public:
    explicit BlackWitch(QWidget *parent, int which_path, Map* map, int step = 0);
    void start_move();
    void stop_move();
    void move_once();
    void attack();
    void start_call();
    void die();
    QLabel* gif = nullptr;
    QMovie* movie = nullptr;
    QPropertyAnimation* animation = nullptr;
    QPropertyAnimation* animation2 = nullptr;
    int which_path = 0;
    bool started = false;
};

#endif // ENEMY_H













