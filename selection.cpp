#include "selection.h"
#include<QPushButton>
#include<QPalette>
#include<QHBoxLayout>
#include<QTimer>
#include<mainwindow.h>
#include<QAbstractAnimation>
#include"defender.h"

Selection::Selection(MainWindow *parent, int who)
    : QPushButton{parent}
{
    setFixedSize(140, 90);
    this->parent = parent;
    //初始化费用说明文字
    QPalette pe;
    pe.setColor(QPalette::ButtonText, Qt::white);
    this->setPalette(pe);
    this->who = who;
    //通过switch case来加载不同图片
    switch (who) {
    case Defender::Boji:{
        name = "boji";
        QPixmap pix;
        pix.load(":/res/boji.png");
        //设置不规则图片样式
        this->setStyleSheet("QPushButton{border:0px;}");
        setIcon(pix);
        setIconSize(QSize(90, 90));
        //设置说明文字
        this->setText("波吉\n消耗：\n100");
        unsufficient(100);
        break;
    }
    case Defender::Witch:{
        name = "witch";
        QPixmap pix;
        pix.load(":/res/witch.png");
        //设置不规则图片样式
        this->setStyleSheet("QPushButton{border:0px;}");
        setIcon(pix);
        setIconSize(QSize(90, 90));
        //设置说明文字
        this->setText("女巫\n消耗：\n200");
        unsufficient(200);
        break;
    }
    case Defender::EvilWizard:{
        name = "EvilWizard";
        QPixmap pix;
        pix.load(":/res/EvilWizard.png");
        //设置不规则图片样式
        this->setStyleSheet("QPushButton{border:0px;}");
        setIcon(pix);
        setIconSize(QSize(90, 80));
        //设置说明文字
        this->setText("邪恶法师\n消耗：\n300");
        unsufficient(300);
        break;
    }
    case Defender::Droid:{
        name = "Droid";
        QPixmap pix;
        pix.load(":/res/Droid.png");
        //设置不规则图片样式
        this->setStyleSheet("QPushButton{border:0px;}");
        setIcon(pix);
        setIconSize(QSize(90, 90));
        //设置说明文字
        this->setText("采集器\n消耗：\n100");
        unsufficient(100);
        break;
    }
    }
}

void Selection::cancel(){
    has_selected = false;
    QString buffer = ":/res/";
    buffer.append(name).append(".png");
    QPixmap pix;
    pix.load(buffer);
    //设置不规则图片样式
    this->setStyleSheet("QPushButton{border:0px;}");
    setIcon(pix);
    setIconSize(QSize(90, 90));
}

void Selection::mouseReleaseEvent(QMouseEvent *ev){
    if(can_select){
        if(!has_selected) {
            has_selected = true;
            QString buffer = ":/res/";
            buffer.append(name).append("_selected.png");
            QPixmap pix;
            pix.load(buffer);
            //设置不规则图片样式
            this->setStyleSheet("QPushButton{border:0px;}");
            setIcon(pix);
            setIconSize(QSize(90, 90));
            Defender* defender = nullptr;
            switch(who){
            case Defender::Boji:
                defender = new Boji(parent);
                parent->set_select(defender, ":/res/boji_coming.png", this);
                break;
            case Defender::Witch:
                defender = new Witch(parent);
                parent->set_select(defender, ":/res/witch_coming.png", this);
                break;
            case Defender::EvilWizard:
                defender = new EvilWizard(parent);
                parent->set_select(defender, ":/res/EvilWizard_coming.png", this);
                break;
            case Defender::Droid:
                defender = new Droid(parent);
                parent->set_select(defender, ":/res/Droid_coming.png", this);
                break;
            }
        }
        else  {
            cancel();
            //取消选择
            parent->set_select(nullptr, "", nullptr);
        }
    }
}
//设置资源不足时，会用黑色icon，并屏蔽按钮
void Selection::unsufficient(int num){
    QTimer::singleShot(200, this, [=](){
        if(((MainWindow*)this->parent)->the_map->get_source() < num){
            can_select = false;
            QString buffer = ":/res/";
            buffer.append(name).append("_no.png");
            QPixmap pix;
            pix.load(buffer);
            //设置不规则图片样式
            this->setStyleSheet("QPushButton{border:0px;}");
            setIcon(pix);
            setIconSize(QSize(90, 90));
        }else if(!has_selected){
            QString buffer = ":/res/";
            buffer.append(name).append(".png");
            QPixmap pix;
            pix.load(buffer);
            //设置不规则图片样式
            this->setStyleSheet("QPushButton{border:0px;}");
            setIcon(pix);
            setIconSize(QSize(90, 90));
            can_select = true;
        }
        unsufficient(num);
    });
}

Spade_Selection::Spade_Selection(MainWindow *parent)
    :   Selection(parent, 999)
{
    //初始化尺寸
    QPixmap pix;
    pix.load(":/res/spade_selection.png");
    pix.scaled(81, 81);
    setFixedSize(81, 81);
    this->setStyleSheet("QPushButton{border:0px;}");
    setIcon(pix);
    setIconSize(QSize(81, 81));
    this->parent = parent;

}

void Spade_Selection::mouseReleaseEvent(QMouseEvent *ev){
    if(can_select){
        if(!has_selected) {
            has_selected = true;      
            QPixmap pix;
            //初始化spade图片
            this->spade = new QLabel();
            this->spade->setParent(parent);
            this->spade->setFixedSize(81, 81);
            this->spade->setScaledContents(true);
            this->spade->setAttribute(Qt::WA_TransparentForMouseEvents);
            pix.load(":/res/spade_coming.png");
            this->spade->setPixmap(pix);
            this->spade->hide();
            this->spade->topLevelWidget();
            //去除铲子之后的按钮标志
            pix.load(":/res/spade_selected");
            pix.scaled(81, 81);
            //设置不规则图片样式
            this->setStyleSheet("QPushButton{border:0px;}");
            setIcon(pix);
            setIconSize(QSize(81, 82));
            spade->move(ev->x() + this->x() - 11, ev->y() + this->y() - 70);
            this->spade->show();
            parent->set_select(nullptr, "", this);
            parent->set_spade(this->spade);
            parent->change_remove(true);
        }
        else  {
            cancel();
            parent->set_select(nullptr, "", nullptr);
            parent->change_remove(false);
        }
    }
}

void Spade_Selection::cancel(){
    has_selected = false;
    QPixmap pix;
    pix.load(":/res/spade_selection.png");
    pix.scaled(81, 81);
    setIcon(pix);
    this->spade->deleteLater();
    this->spade = nullptr;
    parent->set_spade(spade);
}








