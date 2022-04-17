#ifndef DEFENDER_H
#define DEFENDER_H

#include <QWidget>
#include<QLabel>
#include<QPropertyAnimation>

class Block;
class Defender : public QWidget
{
    Q_OBJECT
public:
    //枚举所有我方单位
    enum Defenders{Boji, Witch, EvilWizard, Droid};
    explicit Defender(QWidget *parent = nullptr);
    virtual void add(Block* place);
    virtual int get_cost();
    void health_decrease(int n, int time);
    virtual void die();
    bool is_fighter();
    void remove_defender();
    void mouseReleaseEvent(QMouseEvent* event);
protected:
    int health = 0;
    int damage = 0;
    int cost = 0;
    bool fighter = false;
    Block* place;
    QWidget* parent;
signals:

};

class Boji : public Defender{
public:
    explicit Boji(QWidget *parent);
    void paintEvent(QPaintEvent *);
    int get_cost();
    void add(Block* place);
//    void attack();
//    void enterEvent(QEnterEvent *evnet);
};

class Witch : public Defender{
    int range = 300;
    QLabel *gif = nullptr;
    QLabel *bang = nullptr;
    QLabel *range_circle = nullptr;
    bool bounce_out = false;
    QPropertyAnimation *animation1 = nullptr;
    QPropertyAnimation *animation2 = nullptr;
public:
    explicit Witch(QWidget *parent);
    int get_cost();
    void add(Block* place);
    void attack();
    void die();
    bool eventFilter(QObject* obj, QEvent* event);
};

class EvilWizard : public Defender{
    QLabel *gif = nullptr;
    QMovie* movie = nullptr;
public:
    explicit EvilWizard(QWidget *parent);
    int get_cost();
    void add(Block* place);
    void attack();
    void die();
};

class Droid : public Defender{
    QLabel *gif = nullptr;
    QMovie* movie = nullptr;
public:
    explicit Droid(QWidget *parent);
    int get_cost();
    void add(Block* place);
    void attack();
    void die();
};

#endif // DEFENDER_H