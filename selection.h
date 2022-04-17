#ifndef SELECTION_H
#define SELECTION_H

#include <QWidget>
#include<QPushButton>
#include<QLabel>

class MainWindow;
class Selection : public QPushButton
{
    Q_OBJECT
protected:
    bool has_selected = false;
    bool can_select = true;
    QLabel *black;
    QString name = "";
    int who = 0;
public:
    explicit Selection(MainWindow *parent, int which);
    MainWindow* parent = nullptr;
    virtual void mouseReleaseEvent(QMouseEvent* ev);
    void unsufficient(int num);
    virtual void cancel();
signals:
    void over();
};

class Spade_Selection : public Selection{
    QLabel *spade = nullptr;
    void mouseReleaseEvent(QMouseEvent* ev);
public:
    explicit Spade_Selection(MainWindow *parent);
    void cancel();
};

#endif // SELECTION_H
