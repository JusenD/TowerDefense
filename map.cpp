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
bool Map::initialized = false;
void  Map::init(){            //所有地图初始化
    if(!initialized){
        initialized = true;
        ifstream all_maps("./Map.txt", ios::in);
        char buffer[1000];
        int ways_cnt = 0;
        int way_type = 0;
        //略过第一行说明
        all_maps.getline(buffer, 1000);
        //读取第一行中属性数据,health, original_source, source_speed, add_once, row, colomn
        all_maps.getline(buffer, 1000);
        while(!all_maps.eof()){
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
//    int row = 0, colomn = 0;
//    //一张15x10地图
//    amap first_map;
//    //初始生命值
//    first_map.health = 5000;
//    //设置初始资源和增长速度
//    first_map.original_source = 10000;
//    first_map.source_speed = 6000;
//    first_map.add_once = 50;
//    row = 10; colomn = 15;
//    first_map.row = row;
//    first_map.colomn = colomn;
//    first_map.array = new int[row*colomn];
//    memset(first_map.array, 0, colomn*row * 4);
//    vector<coordinate> way;
//    //设置了一条路径，是否可以用一个比较通用的方法 对啊用 new + for(某个函数可以这样搞)
//    coordinate r1(14, 0);coordinate r2(13, 0);coordinate r3(12, 0);
//    coordinate r4(12, 1);coordinate r5(11, 1);coordinate r6(10, 1);
//    coordinate r7(9, 1);coordinate r8(8, 1);coordinate r9(8, 2);
//    coordinate r10(7, 2);coordinate r11(6, 2);coordinate r12(5, 2);
//    coordinate r16(5, 3);coordinate r17(4, 3);coordinate r18(3, 3);
//    coordinate r19(3, 4);coordinate r20(2, 4);coordinate r21(1, 4);
//    coordinate r22(0, 4);
//    way.push_back(r1);way.push_back(r2);way.push_back(r3);
//    way.push_back(r4);way.push_back(r5);way.push_back(r6);
//    way.push_back(r7);way.push_back(r8);way.push_back(r9);
//    way.push_back(r10);way.push_back(r11);way.push_back(r12);
//    way.push_back(r16);way.push_back(r17);way.push_back(r18);
//    way.push_back(r19);way.push_back(r20);way.push_back(r21);
//    way.push_back(r22);
//    Single_Path* path = new Single_Path(way);
//    //设置路径1为地面路径
//    path->ground = true;
//    //将路径所在处设为1
//    if(path->ground){
//        for(int i = 0; i < way.size(); i++){
//            first_map.array[way[i].y * colomn + way[i].x] = 1;  //等于1表示地面路径
//        }
//    }
//    //设置第二条路经
//    vector<coordinate> way2;
//    for(int i = 0; i < row; i++){
//        coordinate *buffer1 = new coordinate(14-i, i);
//        coordinate *buffer2 = new coordinate(13-i, i);
//        way2.push_back(*buffer1);
//        way2.push_back(*buffer2);
//    }
//    Single_Path* path2 = new Single_Path(way2);
//    path2->ground = true;
//    if(path->ground){
//        for(int i = 0; i < way2.size(); i++){
//            first_map.array[way2[i].y * colomn + way2[i].x] = 1;
//        }
//    }
//    //设置第三条路经
//    vector<coordinate> way3;
//    for(int i = row-1; i >= 0; i--){
//        coordinate *buffer1 = new coordinate(i+3, i);
//        coordinate *buffer2 = new coordinate(i+2, i);
//        way3.push_back(*buffer1);
//        way3.push_back(*buffer2);
//    }
//    Single_Path* path3 = new Single_Path(way3);
//    path3->ground = false;
//    if(path3->ground){
//        for(int i = 0; i < way3.size(); i++){
//            if(!first_map.array[way3[i].y * colomn + way3[i].x])
//            first_map.array[way3[i].y * colomn + way3[i].x] = 2;    //等于2表示飞行路径
//        }
//    }

//    first_map.path.push_back(*path);
//    first_map.path.push_back(*path2);
//    first_map.path.push_back(*path3);
//    all.push_back(first_map);

//    //一张10x10地图
//    row = 10; colomn = 10;
//    amap second_map;
//    //初始生命值
//    second_map.health = 300;
//    //设置初始资源和增长速度
//    second_map.original_source = 0;
//    second_map.source_speed = 3000;
//    second_map.add_once = 50;
//    second_map.row = row;
//    second_map.colomn = colomn;
//    second_map.array = new int[row*colomn];
//    memset(second_map.array, 0, row*colomn*4);
//    //设置第一条路经
//    way.clear();
//    for(int i = 0; i < row - 1; i++){
//        coordinate *buffer1 = new coordinate(9-i, i);
//        coordinate *buffer2 = new coordinate(8-i, i);
//        way.push_back(*buffer1);
//        way.push_back(*buffer2);
//    }
//    coordinate *buffer1 = new coordinate(0, 9);
//    way.push_back(*buffer1);
//    for(int i = 0; i < way.size(); i++){
//        second_map.array[way[i].y * colomn + way[i].x] = 1;
//    }
//    path = new Single_Path(way);
//    second_map.path.push_back(*path);
//    all.push_back(second_map);

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
//            //将路径图编码，对不同情况画不同图片
//            int x = 0;
//            //处理路径四个角
//            if(array[i*colomn + j]&&((j>1 && array[i*colomn + j - 1] == 1) || (i>0&&array[(i-1)*colomn + j] == 1))&&!(i>0&&j>1&&array[i*colomn + j - 1] == 1&&array[(i-1)*colomn + j] == 1&&array[(i-1)*colomn + j-1] == 1)){
//                x+=1000;
//            }
//            if(array[i*colomn + j]&&((j<colomn-1 && array[i*colomn + j + 1] == 1) || (i>0&&array[(i-1)*colomn + j] == 1))&&!(i>0&&j<colomn-1&&array[i*colomn + j + 1] == 1&&array[(i-1)*colomn + j] == 1&&array[(i-1)*colomn + j+1] == 1)){
//                x+=100;
//            }
//            if(array[i*colomn + j]&&((j>1 && array[i*colomn + j - 1] == 1) || (i<row-1&&array[(i+1)*colomn + j] == 1))&&!(i<row-1&&j>1&&array[i*colomn + j - 1] == 1&&array[(i+1)*colomn + j] == 1&&array[(i+1)*colomn + j-1] == 1)){
//                x+=10;
//            }
//            if(array[i*colomn + j]&&((j<colomn-1 && array[i*colomn + j + 1] == 1) || (i<row&&array[(i+1)*colomn + j] == 1))&&!(i<row-1&&j<colomn-1&&array[i*colomn + j + 1] == 1&&array[(i+1)*colomn + j] == 1&&array[(i+1)*colomn + j+1] == 1)){
//                x+=1;
//            }
                                    //画路径
 //           Road* a_road = new Road(parent, j*70, i*70, x);
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
void Map::add_enemy(QWidget *parent, int which_path, int who, int step){
    if(have_not_defeat && (step < (*this->path)[which_path].way.size())){
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
        }
    //    Enemy* a_enemy = new Enemy(parent, &(*this->path)[which_path], this);
        if(!a_enemy->is_ground() || (*this->path)[which_path].ground){
            a_enemy->show();
            a_enemy->start_move();
            this->all_enemy.push_back(a_enemy);
        }
        else a_enemy->~Enemy();
    }
}

//添加我方单位
void Map::add_defender(Block *where, Defender *single_defender){
    if(have_not_defeat){
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
        if(have_not_defeat){
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
    have_not_defeat = false;
    QMessageBox::information(parent, "info", "Defeat");
    for(int i = 0; i < this->all_enemy.size();i++) all_enemy[i]->stop();
    for(int j = 0; j < this->all_defender.size(); j++) all_defender[j]->stop();
}













