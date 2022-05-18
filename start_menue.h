#ifndef START_MENUE_H
#define START_MENUE_H

#include<QMainWindow>
#include"map.h"

class Start_menue : public QMainWindow
{
    Q_OBJECT
public:
    explicit Start_menue(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent* event);
    MainWindow* challenge = nullptr;
signals:

public slots:
};

#endif // START_MENUE_H
