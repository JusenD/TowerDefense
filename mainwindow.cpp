#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"map.h"
#include"enemy.h"
#include<QTimer>
#include<QPainter>
#include"block.h"
#include"selection.h"
#include<QMovie>
#include<QDebug>
#include"detail.h"

MainWindow::MainWindow(int which_map, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    centralWidget()->setAttribute(Qt::WA_TransparentForMouseEvents);
    //添加地图
    the_map = new Map(this, which_map);
    //根据地图设置窗口大小
    setFixedSize(70*the_map->get_colomn(), 70*the_map->get_row()+100);
//    resize(1000, 700);
    setWindowTitle("今日方舟");
    setWindowIcon(QIcon(":/res/icon.jpg"));
    setMouseTracking(true);   
    installEventFilter(this);
    //绑定初始生命值
    this->health = the_map->get_health();
    health_remain = new QProgressBar(this);
    health_remain->setRange(0, *health);
    health_remain->setFormat("%v");
    health_remain->move(0, 0);
    health_remain->setValue(*health);
    //显示资源
    QTimer* source_health_clk = new QTimer(this);
    connect(source_health_clk, &QTimer::timeout, this, [=](){
        display_source_health();
    });
    source_health_clk->start(100);
    ui->menu->move(0, 70*the_map->get_row());

    //添加波吉 selection
    Selection *boji = new Selection(this, Defender::Boji);
    boji->move(85, 70*the_map->get_row() + 5);
    boji->show();

    //添加女巫 selection
    Selection *witch = new Selection(this, Defender::Witch);
    witch->move(215, 70*the_map->get_row() + 5);
    witch->show();

    //添加邪恶法师 selection
    Selection *evilWizard = new Selection(this, Defender::EvilWizard);
    evilWizard->move(330, 70*the_map->get_row() + 5);
    evilWizard->show();

    //添加采集器 selection
    Selection *droid = new Selection(this, Defender::Droid);
    droid->move(460, 70*the_map->get_row() + 5);
    droid->show();

    //添加国王 selection
    Selection *king = new Selection(this, Defender::King);
    king->move(590, 70*the_map->get_row() + 5);
    king->show();

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
    painter.drawPixmap(0, 70*the_map->get_row(), 1050, 100, pix);
}

void MainWindow::display_source_health(){ //刷新显示资源
    ui->source->setText(to_string(the_map->get_source()).c_str());
    this->health_remain->setValue(*health);
}

void MainWindow::start_challenge(){
    //添加敌人波数
    the_waves = new Waves(the_map);
    the_waves->stategy();
}

void MainWindow::stop_challenge(){
    the_waves->stop();
    delete the_waves;
    for (auto enemy : the_map->all_enemy) {
        enemy->delete_now();
    }
    for(auto defender : the_map->all_defender){
        defender->delete_now();
    }
    for (auto block : the_map->all_block) {
        block->deleteLater();
    }
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

Map* MainWindow::get_map(){
    return the_map;
}

void MainWindow::set_select(Defender* selected, QString coming, Selection* selec_selection){
    if(this->select_selection != nullptr){
        select_selection->cancel();
    }
    this->selected = selected;
    this->select_selection = selec_selection;
    this->coming = coming;
}

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

//国王的设置召唤位置状态
void MainWindow::enter_call_state(int row, int colomn){
    call_colomn = colomn;
    call_row = row;
    calling = true;
    coming =  ":/res/Soildier_coming.png";
}

bool MainWindow::is_calling(){
    return calling;
}

void MainWindow::cancel_call_state(){
    call_colomn = 0;
    call_row = 0;
    calling = false;
    coming = "";
    selected = nullptr;
}

void MainWindow::closeEvent(QCloseEvent* event){
    emit be_closed();
}

MainWindow::~MainWindow()
{
    delete ui;
}

