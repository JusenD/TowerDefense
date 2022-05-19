#include "start_menue.h"
#include"mainwindow.h"
#include<QPushButton>
#include<QDebug>
#include<QPainter>
#include<QTime>

Start_menue::Start_menue(QWidget *parent) : QMainWindow(parent)
{
    int map_cnt = Map::map_cnt;
    this->setFixedSize(750, 750);
    for(int i = 0; i < map_cnt; i++){
        QPushButton *btn = new QPushButton(this);
        QString text = "第 ";
        text.append(to_string(i+1).c_str()).append(" 关");
        btn->setText(text);
        btn->move(100 + i % 4 * 150, 650 - i/4 * 100);
        connect(btn, &QPushButton::clicked, [=](){
            challenge = new MainWindow(i);
            this->hide();
            Enemy::on_delete = false;
            challenge->show();
            challenge->start_challenge();
            connect(challenge, &MainWindow::be_closed, [=](){
                this->show();
                Enemy::on_delete = true;
                challenge->stop_challenge();
                challenge->deleteLater();
            });
        });
    }
    QTime randtime = QTime::currentTime();
    srand(randtime.msec() + 1000*randtime.second());
}

void Start_menue::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    QPixmap pix;
    QString randpic = ":/res/start/start";
    randpic.append(to_string(rand()%8).c_str()).append(".png");
    pix.load(randpic);
    painter.drawPixmap(0, 0, 750, 750, pix);
}
