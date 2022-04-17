#ifndef BLOCK_H
#define BLOCK_H

#include <QWidget>
#include<QPaintEvent>
#include"defender.h"
#include<QLabel>

class Map;
class Block : public QWidget
{
    Q_OBJECT
public:
    friend Map;
    explicit Block(QWidget *parent, int x, int y);
    void add_defender(Defender* single_defender);
    std::vector<Defender*>* defender_in();
    QLabel *coming_picture;
    bool empty();
    void delete_defender(Defender* defender);
    void mouseReleaseEvent(QMouseEvent *ev);
    void enterEvent(QEnterEvent *evnet);
    void leaveEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent*);
    bool is_grass();
protected:
    QString type = "";
    QWidget* parent;
    std::vector<Defender*> all_defender; 
    bool grass = false;
signals:

};

class Grass : public Block{
public:
    explicit Grass(QWidget* parent, int x, int y);
    void paintEvent(QPaintEvent*);

};

class Road : public Block{
public:
    explicit Road(QWidget* parent,int x, int y, int choice);
    int choice = 0;
    void paintEvent(QPaintEvent*);
};

#endif // BLOCK_H
