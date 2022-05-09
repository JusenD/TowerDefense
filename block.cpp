#include "block.h"
#include<QPainter>
#include"map.h"
#include"mainwindow.h"
#include"selection.h"
#include<QMouseEvent>
#include<QDebug>

Block::Block(QWidget *parent, int x, int y)
    : QWidget{parent}
{
    this->parent = parent;
    this->setFixedSize(70, 70);
    //初始化coming_picture
    coming_picture = new QLabel(parent);
    coming_picture->setAttribute(Qt::WA_TransparentForMouseEvents);
    coming_picture->raise();
    coming_picture->setScaledContents(true);
    coming_picture->setFixedSize(70, 100);
    coming_picture->move(x, y - 30);
}

void Block::mouseReleaseEvent(QMouseEvent *ev){
    //添加选中的我方单位
    qDebug()<<"鼠标释放了！";
    if(((MainWindow*)parent)->selected != nullptr){
        add_defender(((MainWindow*)parent)->selected);
        ((MainWindow*)parent)->selected = nullptr;
        ((MainWindow*)parent)->coming = "";
    }
    if(((MainWindow*)parent)->select_selection) emit ((MainWindow*)parent)->select_selection->mouseReleaseEvent(ev);
}

//绘制即将选中的悬停效果
void Block::enterEvent(QEvent *event){
 //   qDebug()<<"鼠标进入了！";
    if(((MainWindow*)parent)->coming != ""){
        if((type == "Road" && ((MainWindow*)parent)->selected->is_fighter())
                ||(type == "Grass" && !((MainWindow*)parent)->selected->is_fighter())){
            //设置图案
            QPixmap pix;
            pix.load(((MainWindow*)parent)->coming);
            coming_picture->setPixmap(pix);
            coming_picture->show();
        }
    }
}
void Block::leaveEvent(QEvent *event){
//     qDebug()<<"鼠标离开了！";
    coming_picture->hide();
}


bool Block::empty(){
    return this->all_defender.empty();
}

void Block::add_defender(Defender* single_defender){
    if(type == "Road" && single_defender->is_fighter()){
        ((MainWindow*)(this->parent))->the_map->add_defender(this, single_defender);
    }
    else if(type == "Grass" && !single_defender->is_fighter()){
        ((MainWindow*)(this->parent))->the_map->add_defender(this, single_defender);
    }
}

void Block::push_a_defender(Defender* single_defender){
    this->all_defender.push_back(single_defender);
}

void Block::delete_defender(Defender* defender){
    std::vector<Defender*>::iterator itor;
    for (itor = all_defender.begin(); itor != all_defender.end(); itor++)
        {
            if (*itor == defender)
            {
                itor = all_defender.erase(itor);
                break;
            }
        }
}

std::vector<Defender*>* Block::defender_in(){
    return &this->all_defender;
}

bool Block::is_grass(){return grass;}

void Block::not_grass(){grass = false;}

Grass::Grass(QWidget* parent, int x, int y)
    : Block{parent, x, y}
{
    grass = true;
    this->parent = parent;
    this->type = "Grass";
    //移动到该方位
    this->move(x, y);
}

Road::Road(QWidget* parent, int x, int y, int choice)
    : Block{parent, x, y}
{
    grass = false;
    this->parent = parent;
    this->choice = choice;
    this->type = "Road";
    //移动到该方位
    this->move(x, y);
}

void Block::paintEvent(QPaintEvent *){
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/grass.png");
    painter.drawPixmap(0, 0, 70, 70, pix);
}

void Grass::paintEvent(QPaintEvent *){
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/grass.png");
    painter.drawPixmap(0, 0, 70, 70, pix);
}

void Road::paintEvent(QPaintEvent *){
    QPainter painter(this);
    QPixmap pix;
    QString pic = ":/res/";
    pic.append(to_string(this->choice).c_str()).append(".png");
    pix.load(pic);
    painter.drawPixmap(0, 0, 70, 70, pix);
}







