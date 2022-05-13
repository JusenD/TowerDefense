#ifndef HEALTHBAR_H
#define HEALTHBAR_H

#include <QLabel>
#include"defender.h"
#include"enemy.h"

class healthBar : public QLabel
{
    Q_OBJECT
    int total = 0;
    int type = 0;           //0表示敌方血条..只能这样了
    Enemy* enemy_parent;
    Defender* defender_parent;
public:
    explicit healthBar(Enemy* enemy);
    explicit healthBar(Defender* defender);
    void paintEvent(QPaintEvent* event);

signals:

public slots:
};

#endif // HEALTHBAR_H
