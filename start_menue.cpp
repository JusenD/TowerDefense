#include "start_menue.h"
#include"mainwindow.h"
#include<QPushButton>
#include<QDebug>
#include<QPainter>
#include<QTime>
#include"addwindow.h"
#include"editwindow.h"

Start_menue::Start_menue(QWidget *parent) : QMainWindow(parent)
{
    int map_cnt = Map::map_cnt;
    this->setFixedSize(750, 750);
    //关卡按钮
    for(int i = 0; i < map_cnt; i++){
        QPushButton *btn = new QPushButton(this);
        QString text = "第 ";
        text.append(to_string(i+1).c_str()).append(" 关");
        btn->setText(text);
        btn->move(100 + i % 4 * 150, 550 - i/4 * 100);
        connect(btn, &QPushButton::clicked, [=](){
            challenge = new MainWindow(i);
            this->hide();
            Enemy::on_delete = false;
            Defender::on_delete = false;
            challenge->show();
            challenge->start_challenge();
            connect(challenge, &MainWindow::be_closed, [=](){
                this->show();
                Enemy::on_delete = true;
                Defender::on_delete = true;
                challenge->stop_challenge();
                challenge->deleteLater();
            });
        });
        all_btn.push_back(btn);
    }
    QTime randtime = QTime::currentTime();
    srand(randtime.msec() + 1000*randtime.second());
    //新建地图按钮
    QPushButton *add_map_btn = new QPushButton(this);
    add_map_btn->setText("新建地图");
    add_map_btn->move(250, 650);
    connect(add_map_btn, &QPushButton::released, this, [=](){
        AddWindow* addwindow = new AddWindow();
        this->hide();
        addwindow->show();
        connect(addwindow, &AddWindow::on_close, this, [=](){
            this->update_maps();
            this->show();
        });
    });
    //编辑地图按钮
    QPushButton *edit_map_btn = new QPushButton(this);
    edit_map_btn->setText("编辑地图");
    edit_map_btn->move(400, 650);
    connect(edit_map_btn, &QPushButton::released, this, [=](){
        EditWindow* editwindow = new EditWindow(nullptr, &(Map::all));
        this->hide();
        editwindow->show();
        connect(editwindow, &EditWindow::on_close, this, [=](){
            this->update_maps();
            this->show();
        });
    });
}

void Start_menue::update_maps(){
    for(auto btn : all_btn) btn->deleteLater();
    all_btn.clear();
    Map::reinit();
    int map_cnt = Map::map_cnt;
    //关卡按钮
    for(int i = 0; i < map_cnt; i++){
        QPushButton *btn = new QPushButton(this);
        QString text = "第 ";
        text.append(to_string(i+1).c_str()).append(" 关");
        btn->setText(text);
        btn->move(100 + i % 4 * 150, 550 - i/4 * 100);
        connect(btn, &QPushButton::clicked, [=](){
            challenge = new MainWindow(i);
            this->hide();
            Enemy::on_delete = false;
            Defender::on_delete = false;
            challenge->show();
            challenge->start_challenge();
            connect(challenge, &MainWindow::be_closed, [=](){
                this->show();
                Enemy::on_delete = true;
                Defender::on_delete = true;
                challenge->stop_challenge();
                challenge->deleteLater();
            });
        });
        all_btn.push_back(btn);
    }
}

void Start_menue::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    QPixmap pix;
    QString randpic = ":/res/start/start";
    randpic.append(to_string(rand()%8).c_str()).append(".png");
    pix.load(randpic);
    painter.drawPixmap(0, 0, 750, 750, pix);
}
