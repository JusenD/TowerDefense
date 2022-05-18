#include "start_menue.h"
#include"mainwindow.h"
#include<QPushButton>
#include<QDebug>

Start_menue::Start_menue(QWidget *parent) : QMainWindow(parent)
{
    int map_cnt = Map::map_cnt;
    this->setFixedSize(750, 600);
    for(int i = 0; i < map_cnt; i++){
        QPushButton *btn = new QPushButton(this);
        QString text = "第 ";
        text.append(to_string(i+1).c_str()).append(" 关");
        btn->setText(text);
        btn->move(100 + i % 4 * 150, 500 - i/4 * 100);
        connect(btn, &QPushButton::clicked, [=](){
            MainWindow* challenge = new MainWindow(i);
            this->hide();
            challenge->show();
            challenge->start_challenge();
            connect(challenge, &MainWindow::be_closed, [=](){
                this->show();
            });
        });
    }
}
