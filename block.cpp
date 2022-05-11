#include "block.h"
#include<QPainter>
#include"map.h"
#include"mainwindow.h"
#include"selection.h"
#include<QMouseEvent>
#include<QDebug>

Block::Block(QWidget *parent, int colomn_now, int row_now)
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
    coming_picture->move(colomn_now*70, row_now*70 - 30);
    the_map = ((MainWindow*)parent)->the_map;

    this->row_now = row_now;
    this->colomn_now = colomn_now;
}

void Block::mouseReleaseEvent(QMouseEvent *ev){
    //添加选中的我方单位
    qDebug()<<"鼠标释放了！";
    //将悬停图片清除
    coming_picture->clear();
    //选择国王召唤位置状态
    if(((MainWindow*)parent)->calling){
        int r = ((MainWindow*)parent)->call_row, c = ((MainWindow*)parent)->call_colomn;
        //判断九宫格
        if(abs(r - this->row_now) <= 1 && abs(c - this->colomn_now) <= 1 && !(r == this->row_now && c== this->colomn_now)){
            ((King*)((MainWindow*)parent)->selected)->setTarget(this);
            ((MainWindow*)parent)->cancel_call_state();
        }
    }
    else if(((MainWindow*)parent)->selected != nullptr){
        add_defender(((MainWindow*)parent)->selected);
        if(((MainWindow*)parent)->selected->is_king()){
            ((MainWindow*)parent)->enter_call_state(this->row_now, this->colomn_now);
        }
        else{
            ((MainWindow*)parent)->selected = nullptr;
            ((MainWindow*)parent)->coming = "";
        }
    }
    if(((MainWindow*)parent)->select_selection) emit ((MainWindow*)parent)->select_selection->mouseReleaseEvent(ev);
}

//绘制即将选中的悬停效果
void Block::enterEvent(QEvent *event){
 //   qDebug()<<"鼠标进入了！";
    if(((MainWindow*)parent)->calling){
        int r = ((MainWindow*)parent)->call_row, c = ((MainWindow*)parent)->call_colomn;
        //判断九宫格
        if(abs(r - this->row_now) <= 1 && abs(c - this->colomn_now) <= 1 && !(r == this->row_now && c== this->colomn_now)){
            QPixmap pix;
            pix.load(((MainWindow*)parent)->coming);
            coming_picture->setPixmap(pix);
            coming_picture->show();
        }
    }
    else if(((MainWindow*)parent)->coming != ""){
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

Grass::Grass(QWidget* parent, int colomn_now, int row_now)
    : Block{parent, colomn_now, row_now}
{
    grass = true;
    this->parent = parent;
    this->type = "Grass";
    //移动到该方位
    this->move(colomn_now*70, row_now*70);
}

Road::Road(QWidget* parent, int colomn_now, int row_now, int choice)
    : Block{parent, colomn_now, row_now}
{
    grass = false;
    this->parent = parent;
    this->choice = choice;
    this->type = "Road";
    //移动到该方位
    this->move(colomn_now*70, row_now*70);
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







