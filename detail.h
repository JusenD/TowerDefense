#ifndef DETAIL_H
#define DETAIL_H

#include <QLabel>
#include <QPropertyAnimation>

class Enemy;
class Detail : public QLabel
{
    Q_OBJECT
    int attach_x = 0;
    int attach_y = 0;
    int* health = nullptr;
    int original_health = 0;
    int* damage = nullptr;
    int original_damage = 0;
    QLabel* health_text = nullptr;
    QLabel* damage_text = nullptr;
    QPropertyAnimation* detail_animation1 = nullptr;
    QPropertyAnimation* detail_animation2 = nullptr;
    bool is_enemy = false;
    bool special_PLUS = false;
    bool on_disappear = false;
    QLabel* follow = nullptr;
//    int timerId1 = 0;
//    int timerId2 = 0;
public:
    explicit Detail(QWidget *parent, int x, int y, int* health, int* damage, int original_health, int original_damage);
    explicit Detail(QWidget *parent, QLabel* follow, int* health, int* damage, int original_health, int original_damage, bool PLUS = false);
    void timerEvent(QTimerEvent* event);
    void appear();
    void disappear();
public slots:
};

#endif // DETAIL_H
