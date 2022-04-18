#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"map.h"
#include"enemy.h"
#include<QTimer>
#include<QPainter>
#include"block.h"
#include"selection.h"
#include<QMovie>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    centralWidget()->setAttribute(Qt::WA_TransparentForMouseEvents);
    //添加地图
    the_map = new Map(this, 0);
    //根据地图设置窗口大小
    setFixedSize(70*the_map->get_colomn(), 70*the_map->get_row()+100);
//    resize(1000, 700);
    setWindowTitle("今日方舟");
    setWindowIcon(QIcon(":/res/icon.jpg"));
    setMouseTracking(true);   
    installEventFilter(this);
    //显示资源
    display_source();
    //添加一个敌人
    the_map->add_enemy(this, 0, Enemy::Daida);

    //再添加一堆敌人

    QTimer::singleShot(1000, this, [=](){
        the_map->add_enemy(this, 1, Enemy::Bat);
    });
    QTimer::singleShot(2000, this, [=](){
        the_map->add_enemy(this, 0, Enemy::Skeleton);
    });
    QTimer::singleShot(3000, this, [=](){
        the_map->add_enemy(this, 0, Enemy::Bat);
    });
    QTimer::singleShot(4000, this, [=](){
        the_map->add_enemy(this, 1, Enemy::Daida);
    });
    QTimer::singleShot(5000, this, [=](){
        the_map->add_enemy(this, 1, Enemy::Skeleton);
    });
    //尝试添加一个骷髅兵
//    Daida* a_daida = new Daida(this, &(*the_map->path)[0], the_map);
//    a_daida->start_move();
//    Skeleton* a_skeleton = new Skeleton(this, &(*the_map->path)[0], the_map);
//    a_skeleton->start_move();
    the_map->add_enemy(this, 0, Enemy::Skeleton);

//    //尝试添加一个蝙蝠
//    Bat* a_bat = new Bat(this, &(*the_map->path)[0], the_map);
//    a_bat->start_move();
    the_map->add_enemy(this, 0, Enemy::Bat);

    //尝试添加收集器
    Droid* a_droid = new Droid(this);
    the_map->all_block[23]->add_defender(a_droid);

    //添加波吉 selection
    Selection *boji = new Selection(this, Defender::Boji);
    boji->move(85, 705);
    boji->show();

    //添加女巫 selection
    Selection *witch = new Selection(this, Defender::Witch);
    witch->move(215, 705);
    witch->show();

    //添加邪恶法师 selection
    Selection *evilWizard = new Selection(this, Defender::EvilWizard);
    evilWizard->move(330, 705);
    evilWizard->show();

    //添加采集器 selection
    Selection *droid = new Selection(this, Defender::Droid);
    droid->move(460, 705);
    droid->show();


//    QPixmap pix;
//    pix.load(":/res/spade_selection.png");
//    //设置图片固定大小
//    ui->spadeButton->setFixedSize(81, 81);
//    //设置不规则图片样式
//    ui->spadeButton->setStyleSheet("QPushButton{border:0px;}");
//    //设置图标
//    ui->spadeButton->setIcon(pix);
//    //设置图标大小
//    ui->spadeButton->setIconSize(QSize(81, 81));
   //添加铲子 selection
    Selection *spade_selection = new Spade_Selection(this);
    spade_selection->move(960-1, 70*the_map->get_row() + 9);
    spade_selection->show();

}

void MainWindow::paintEvent(QPaintEvent*){
    //画菜单
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/menu.png");
    painter.drawPixmap(0, 700, 1050, 100, pix);
}

void MainWindow::display_source(){ //递归调用每0.2s刷新显示资源（感觉这样做不太好，如果有某种捆绑表示最好）
    ui->source->setText(to_string(the_map->get_source()).c_str());
    QTimer::singleShot(200, this, [=](){
        display_source();
    });
}

QString MainWindow::get_coming(){
    return coming;
}
Defender* MainWindow::get_selected(){
    return selected;
}

void MainWindow::set_spade(QLabel* spade){
    this->spade = spade;
}

void MainWindow::change_remove(bool whether){
    remove_now = whether;
}
bool MainWindow::can_remove(){
    return remove_now;
}

Selection* MainWindow::get_selection(){
    return select_selection;
}

void MainWindow::set_select(Defender* selected, QString coming, Selection* selec_selection){
    if(this->select_selection != nullptr){
        select_selection->cancel();
    }
    this->selected = selected;
    this->select_selection = selec_selection;
    this->coming = coming;
}

//void MainWindow::mouseMoveEvent(QMouseEvent* ev){
//    qDebug()<<"鼠标移动了！";
//    if(spade){
//        spade->move(ev->x(), ev->y());
//        qDebug()<<ev->x()<<ev->y();
//    }
//}

//追踪鼠标位置移动铲子
bool MainWindow::eventFilter(QObject* object, QEvent* event){
//    qDebug()<<"MainWindow再拦截";
//    qDebug()<<event->type();
//    if(event->type() == QEvent::GraphicsSceneMouseMove)
    if(event->type() == QEvent::HoverMove)
      {
//      qDebug()<<"进来了!!!!!!!";
        if(spade){
        QMouseEvent *mouse_ev = static_cast<QMouseEvent *>(event);
        spade->move(mouse_ev->x() - 11, mouse_ev->y() - 70);
//        qDebug()<<mouse_ev->x()<<mouse_ev->y();
        }
      }
      return QMainWindow::eventFilter(object, event);
}

MainWindow::~MainWindow()
{
    delete ui;
}

