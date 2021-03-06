#ifndef ENEMY_H
#define ENEMY_H

#include <QWidget>
#include "path.h"
#include "block.h"
#include <QPropertyAnimation>
#include <QTimer>
#include "detail.h"

class MainWindow;
class Map;
class healthBar;
class Enemy : public QWidget
{
    Q_OBJECT
public:
    //枚举所有敌人种类
    enum Enemies{Daida, Bat, Skeleton, BlackWitch, Bot, Plus};
    //当前位置
    int x_now;
    int y_now;
    Block* block_now = nullptr;
    //传入地图
    Map* map;
    //-1表示未进场
    int step = -1;
    virtual void start_move() = 0;
    virtual void move_once() = 0;
    virtual void attack() = 0;
    virtual void delete_now() = 0;
    void stop();
    virtual void health_decrease(int n, int time);
    virtual void die() = 0;
    bool is_ground();
    bool is_fly();
    int get_x();
    int get_y();
    int get_health();
    int get_original_health();
    void end();
    void delete_enemy();
    static bool all_enemy_access;
    QLabel* get_gif();
    static void cut_off(QLabel* gif,QMovie* movie, int time);
    static bool on_delete;
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void buff();
protected:
    //设置生命值
    int original_health = 0;
    int original_damage = 0;
    int health = 0;
    int damage = 0;
    bool can_move = true;
    bool ground = false;
    bool fly = false;
    bool has_dead = false;
    MainWindow* parent = nullptr;
    Single_Path* this_path = nullptr;
    bool unfinished = true;
    healthBar* health_bar;
    Detail* detail = nullptr;
    QLabel* gif = nullptr;
    QMovie* movie = nullptr;
    //时钟信号
    QTimer* move_clk = nullptr;
    QTimer* attack_clk = nullptr;
    QPropertyAnimation* animation = nullptr;
    QPropertyAnimation* animation2 = nullptr;
    bool detail_out = false;
    bool on_buff = false;
    QLabel* buff_gif = nullptr;
    QMovie* buff_movie = nullptr;
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
    void move_once();
    void attack();
    void die();
    void stop();
    void delete_now();
    QPropertyAnimation *animation1 = nullptr;
    QPropertyAnimation *animation3 = nullptr;
    QPropertyAnimation *animation4 = nullptr;
};

class Skeleton: public Ground_Enemy{
public:
    explicit Skeleton(QWidget *parent, int which_path, Map* map, int step = 0);
    void start_move();
    void move_once();
    void attack();
    void buff();
    void die();
    void stop();
    void delete_now();
};

class Bat: public Fly_Enemy{
public:
    explicit Bat(QWidget *parent, int which_path, Map* map, int step = 0);
    void start_move();
    void move_once();
    void attack();
    void buff();
    void die();
    void stop();
    void delete_now();
};

class BlackWitch : public Fly_Enemy{
public:
    explicit BlackWitch(QWidget *parent, int which_path, Map* map, int step = 0);
    void start_move();
    void move_once();
    void attack();
    void buff();
    void start_call();
    void die();
    void stop();
    void delete_now();
    int which_path = 0;
};

class Bot : public Ground_Enemy{
    QLabel* bullet = nullptr;
    QPropertyAnimation* animation3 = nullptr;
    bool on_attack = false;
    bool invincible = false;
    float miss = 0.3;
public:
    explicit Bot(QWidget *parent, int which_path, Map* map, int step = 0);
    void start_move();
    void move_once();
    void attack();
    void buff();
    void health_decrease(int, int);
    void die();
    void stop();
    void delete_now();
    Defender* find_target();
};

class Plus: public Ground_Enemy{
    bool can_attack = false;
public:
    explicit Plus(QWidget *parent, int which_path, Map* map, int step = 0);
    void start_move();
    void move_once();
    void attack();
    void buff();
    void die();
    void stop();
    void delete_now();
    void enable_attack();
    void mouseReleaseEvent(QMouseEvent* event);
};

#endif // ENEMY_H













