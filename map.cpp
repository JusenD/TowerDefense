#include "map.h"
#include<QPainter>
#include"path.h"
#include"block.h"
#include<QLabel>
#include"enemy.h"
#include<QTimer>
#include<QTime>
#include<QMessageBox>
#include<fstream>
#include<QDebug>

vector<amap> Map::all;
int Map::map_cnt = 0;
bool Map::initialized = false;
void  Map::init(){            //所有地图初始化
    if(!initialized){
        initialized = true;
        all.clear();
        map_cnt = 0;
        ifstream all_maps("./Map.txt", ios::in);
        char buffer[1000];
        int ways_cnt = 0;
        int way_type = 0;
        //略过第一行说明
        all_maps.getline(buffer, 1000);
        //读取第一行中属性数据,health, original_source, source_speed, add_once, row, colomn
        all_maps.getline(buffer, 1000);
        while(!all_maps.eof()){
            map_cnt++;
            //创建一张地图
            amap* single_map = new amap;;
            sscanf(buffer, "%d,%d,%d,%d,%d,%d", &single_map->health, &single_map->original_source, &single_map->source_speed, &single_map->add_once, &single_map->row, &single_map->colomn);
 //           qDebug()<<single_map->health<<single_map->original_source<<single_map->source_speed<<single_map->add_once<<single_map->row<<single_map->colomn;
            //创建地图数组并清空
            single_map->array = new int[single_map->row * single_map->colomn];
            memset(single_map->array, 0, single_map->row*single_map->colomn*4);
            //读取下一行数据，共有几条路径
            all_maps.getline(buffer, 1000);
            sscanf(buffer, "%d", &ways_cnt);
            //循环添加路径
            for(int n = 0; n < ways_cnt; n++){
                vector<coordinate> way;
                //读取一行，路径种类
                all_maps.getline(buffer, 1000);
                sscanf(buffer, "%d", &way_type);
                //读取坐标
                all_maps.getline(buffer, 1000);
                string a_way = buffer;
                int i = 0, j = a_way.find(','), cut = a_way.find(' ');
                int x = 0, y = 0;
                while(j != string::npos){
                    x = atoi(a_way.substr(i, cut).c_str());
                    y = atoi(a_way.substr(cut+1, j).c_str());
                    coordinate r(x, y);
                    way.push_back(r);
//                    qDebug()<<'('<<x<<','<<y<<')';
                    i = j+1, j = a_way.find(',', j+1);
                    cut = a_way.find(' ', cut+1);
                }
                x = x = atoi(a_way.substr(i, cut).c_str());
                y = atoi(a_way.substr(cut+1).c_str());
                coordinate r(x, y);
                way.push_back(r);
//                qDebug()<<'('<<x<<','<<y<<')';
                Single_Path* path = new Single_Path(way);
                if(way_type == 1){
                    path->ground = true;
                    //将路径所在处设为 路径种类
                    for(int i = 0; i < way.size(); i++){
                        single_map->array[way[i].y * single_map->colomn + way[i].x] = way_type;
                    }
                }
                else if(way_type == 2){
                    path->ground = false;
                }

                single_map->path.push_back(*path);
            }
            //捕获end信号
            all_maps.getline(buffer, 1000);
            //读取第一行中属性数据,health, original_source, source_speed, add_once, row, colomn
            all_maps.getline(buffer, 1000);
            all.push_back(*single_map);
        }
    }
}

void Map::reinit(){
    initialized = false;
    init();
}

Map::Map(QWidget* parent, int num){
    //加载初始地图参数
    this->parent = parent;
    this->colomn = all[num].colomn;
    this->row = all[num].row;
    this->array = all[num].array;
    this->path = &all[num].path;
    this->health = all[num].health;
    //初始化背景中的地块按钮
    QPixmap pix;
    for(int i = 0; i < row; i++){
        for(int j = 0; j < colomn; j++){
            if(array[i*colomn + j] == 0 || array[i*colomn + j] == 2){         //画草地
                Grass* a_grass = new Grass(parent, j, i);
                a_grass->show();
                all_block.push_back(a_grass);
                continue;
            }
            Road* a_road = new Road(parent, j, i, 0);
            a_road->show();
            all_block.push_back(a_road);

        }
    }
    //画路径起点
    for(int i = 0; i < (*path).size(); i++){
        if((*path)[i].ground){
            //陆地起点
            pix.load(":/res/ground_start.png");
            pix.scaled(70, 70);
        }
        else{
            //飞行起点
            pix.load(":/res/fly_start.png");
            pix.scaled(70, 70);
        }
        QLabel *cover = new QLabel(parent);
        cover->setFixedSize(70, 70);
        cover->move((*path)[i].way[0].x*70, (*path)[i].way[0].y*70);
        cover->setScaledContents(true);
        cover->setPixmap(pix);
        cover->show();
        all_block[(*path)[i].way[0].y*colomn + (*path)[i].way[0].x]->not_grass();
 //       painter.drawPixmap((*path)[i].way[0].x*70, (*path)[i].way[0].y*70, 70, 70, pix);
    }

    //画装饰，随机在地图上画，不用单独设计，但是真的好丑啊
    QTime randtime = QTime::currentTime();
    srand(randtime.msec()+randtime.second()*1000); //初始化随机数
    int i = all_block.size();
    int step = step = rand()%5+7; //设置多少步添加一次
    //画小装饰
    for(int m = step ; m<i; m += step){
        if(all_block[m]->is_grass()){
            int what = rand()%14 + 1;        //随机选1-11的植物
            QString url = ":/res/decoration/plant";
            url.append(to_string(what).c_str()).append(".png");
            pix.load(url);
            pix.scaled(40, 40);

            QLabel* plant = new QLabel(parent);
            plant->setFixedSize(40, 40);
            if(what == 14) {pix.scaled(35, 70); plant->setFixedSize(35, 70);}
            plant->move(all_block[m]->x()+25, all_block[m]->y()+15);
            plant->setScaledContents(true);
            plant->setPixmap(pix);
            plant->setAttribute(Qt::WA_TransparentForMouseEvents);
            plant->show();
        }
        step = rand()%5+7;
    }
     //画树
    step = rand()%7+5;
    for(int m = step ; m<i; m += step){
        if(all_block[m]->is_grass()){
            int what = rand()%2 + 1;        //随机选1或2的树
            QString url = ":/res/decoration/tree";
            url.append(to_string(what).c_str()).append(".png");
            pix.load(url);
            pix.scaled(55, 70);
            QLabel* tree = new QLabel(parent);
            tree->setFixedSize(55, 70);
            tree->move(all_block[m]->x(), all_block[m]->y());
            tree->setScaledContents(true);
            tree->setPixmap(pix);
            tree->setAttribute(Qt::WA_TransparentForMouseEvents);
            tree->show();
        }
        step = rand()%7+5;
    }


    //初始化资源系统
    this->ADD_ONCE = all[num].add_once;
    this->ADD_TIME = all[num].source_speed;
    this->source = all[num].original_source;
    increase_source();
}

int Map::get_colomn(){return colomn;}

int Map::get_row(){return row;}

//添加敌方单位
Enemy* Map::add_enemy(QWidget *parent, int which_path, int who, int step){
    if(have_not_end && which_path < (*this->path).size() && (step < (*this->path)[which_path].way.size())){
        Enemy* a_enemy = nullptr;
        switch (who) {
        case Enemy::Daida:
            a_enemy = new Daida(parent, which_path, this, step);
            break;
        case Enemy::Skeleton:
            a_enemy = new Skeleton(parent, which_path, this, step);
            break;
        case Enemy::Bat:
            a_enemy = new Bat(parent, which_path, this, step);
            break;
        case Enemy::BlackWitch:
            a_enemy = new BlackWitch(parent, which_path, this, step);
            break;
        case Enemy::Bot:
            a_enemy = new Bot(parent, which_path, this, step);
            break;
        case Enemy::Plus:
            a_enemy = new Plus(parent, which_path, this, step);
            break;
        }
    //    Enemy* a_enemy = new Enemy(parent, &(*this->path)[which_path], this);
        if(!a_enemy->is_ground() || (*this->path)[which_path].ground){
            a_enemy->show();
            a_enemy->start_move();
            this->all_enemy.push_back(a_enemy);
            return a_enemy;
        }
        else a_enemy->delete_now();
        return nullptr;
    }
}

//添加我方单位
void Map::add_defender(Block *where, Defender *single_defender){
    if(have_not_end){
        if(source >= single_defender->get_cost()){
            if(where->all_defender.empty()) {
                source-=single_defender->get_cost();
                where->all_defender.push_back(single_defender);
                single_defender->add(where);
                all_defender.push_back(single_defender);
            }
        }
    }
}

//资源随时间增加，使用递归调用
void Map::increase_source(){
    QTimer::singleShot(ADD_TIME, [=](){
        if(have_not_end){
            source += ADD_ONCE;
            increase_source();
        }
    });
}

int Map::get_source(){
    return this->source;
}

void Map::add_source(int num){
    source += num;
}

int* Map::get_health(){
    return &health;
}

//减少生命值
void Map::decrease_health(int num){
    health -= num;
    if(health <= 0) defeat();
}

void Map::defeat(){
    have_not_end = false;
    for(auto enemy : all_enemy) enemy->stop();
    for(auto defender : all_defender) defender->stop();
    QMessageBox::information(parent, "info", "Defeat");
}

void Map::victory(){
    have_not_end = false;
    for(auto enemy : all_enemy) enemy->stop();
    for(auto defender : all_defender) defender->stop();
    QMessageBox::information(parent, "info", "You have won!");
    for(auto enemy : all_enemy){
        QTimer::singleShot(50, this->parent, [=](){
            enemy->die();
        });
    }
}











