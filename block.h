#ifndef BLOCK_H
#define BLOCK_H

#include <QWidget>
#include<QPaintEvent>
#include"defender.h"
#include<QLabel>

class Map;
class Enemy;
class Block : public QWidget
{
    Q_OBJECT
public:
    friend Map;
    explicit Block(QWidget *parent, int colomn_now, int row_now);
    bool add_defender(Defender* single_defender);
    void push_a_defender(Defender* single_defender);
    void push_an_enemy(Enemy* single_enemy);
    std::list<Defender*>* defender_in();
    std::list<Enemy*>* enemy_in();
    QLabel *coming_picture;
    bool empty();
    void delete_defender(Defender* defender);
    void pop_enemy(Enemy* enemy);
    void mouseReleaseEvent(QMouseEvent *ev);
    void enterEvent(QEvent *evnet);
    void leaveEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent*);
    bool is_grass();
    void not_grass();
protected:
    QString type = "";
    QWidget* parent;
    std::list<Defender*> all_defender;
    std::list<Enemy*> all_enemy;
    bool grass = false;
    Map* the_map = nullptr;
    int row_now = 0;
    int colomn_now = 0;
};

class Grass : public Block{
public:
    explicit Grass(QWidget* parent, int colomn_now, int row_now);
    void paintEvent(QPaintEvent*);

};

class Road : public Block{
public:
    explicit Road(QWidget* parent,int colomn_now, int row_now, int choice);
    int choice = 0;
    void paintEvent(QPaintEvent*);
};

#endif // BLOCK_H
