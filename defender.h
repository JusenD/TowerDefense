#ifndef DEFENDER_H
#define DEFENDER_H

#include <QWidget>
#include<QLabel>
#include<QPropertyAnimation>

class Block;
class healthBar;
class Defender : public QWidget
{
    Q_OBJECT
public:
    //枚举所有我方单位
    enum Defenders{Boji, Witch, EvilWizard, Droid, Soildier, King};
    explicit Defender(QWidget *parent = nullptr);
    virtual void add(Block* place);
    int get_cost();
    int get_health();
    int get_original_health();
    void health_decrease(int n, int time);
    virtual void die();
    bool is_fighter();
    bool is_tower();
    bool is_king();
    void remove_defender();
    void mouseReleaseEvent(QMouseEvent* event);
    void stop();
    virtual QLabel* get_gif();
protected:
    int original_health = 0;
    int health = 0;
    int damage = 0;
    int cost = 0;
    bool fighter = false;
    bool tower = false;
    bool king = false;
    Block* place;
    QWidget* parent;
    bool unfinished = true;
    healthBar* health_bar;
    QLabel *gif = nullptr;
    QMovie* movie = nullptr;
    //时钟信号
    QTimer* attack_clk = nullptr;
signals:

};

class Boji : public Defender{
public:
    explicit Boji(QWidget *parent);
    void paintEvent(QPaintEvent *);
    void add(Block* place);
//    void attack();
//    void enterEvent(QEnterEvent *evnet);
};

class Witch : public Defender{
    int range = 300;
    QLabel *bang = nullptr;
    QLabel *range_circle = nullptr;
    QMovie *bang_movie = nullptr;
    bool bounce_out = false;
    QPropertyAnimation *animation1 = nullptr;
    QPropertyAnimation *animation2 = nullptr;
    int enemy_x = 0, enemy_y = 0;
public:
    explicit Witch(QWidget *parent);
    void add(Block* place);
    void attack();
    void die();
    bool eventFilter(QObject* obj, QEvent* event);
};

class EvilWizard : public Defender{
    QLabel* giff;
public:
    explicit EvilWizard(QWidget *parent);
    void add(Block* place);
    void attack();
    void die();
    QLabel* get_gif();
};

class Droid : public Defender{
public:
    explicit Droid(QWidget *parent);
    void add(Block* place);
    void attack();
    void die();
};

class Soildier : public Defender{
    QPropertyAnimation* animation1 = nullptr;
    QPropertyAnimation* animation2 = nullptr;
public:
    explicit Soildier(QWidget *parent);
    void add(Block* place, int x_now, int y_now);
    void attack();
    void die();
};

class King : public Defender{
    Block* target = nullptr;
public:
    explicit King(QWidget *parent);
    void add(Block* place);
    void attack();
    void die();
    void setTarget(Block* target);
};

#endif // DEFENDER_H
