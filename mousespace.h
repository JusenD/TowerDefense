#ifndef MOUSESPACE_H
#define MOUSESPACE_H

#include <QWidget>
#include<QLabel>

class mouseSpace : public QWidget
{
    Q_OBJECT
public:
    explicit mouseSpace(QWidget *parent, QLabel* spade);
    QLabel* spade = nullptr;
//    void mouseMoveEvent(QMouseEvent* ev);
    bool eventFilter(QObject* object, QEvent* event);
signals:

};

#endif // MOUSESPACE_H
