#include "editwindow.h"
#include "ui_editwindow.h"
#include"pathblock.h"
#include<QMessageBox>
#include<fstream>
#include<QDebug>
#include<QComboBox>

EditWindow::EditWindow(QWidget *parent, vector<amap>* all_maps) :
    QWidget(parent),
    ui(new Ui::EditWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    this->all_maps = all_maps;
    //设置MapBox的选项
    for(int i = 0 ; i < all_maps->size(); i++) {
        string buffer = "第";
        buffer.append(to_string(i+1)).append("关");
        ui->MapBox->addItem(buffer.c_str());
    }
    //选中关卡后更新数据
    connect(ui->MapBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, [=](int i){
        map_selected = i;
        ui->health_box->setValue(all_maps->at(i).health);
        ui->original_source_box->setValue(all_maps->at(i).original_source);
        ui->speed_box->setValue(all_maps->at(i).source_speed / 1000);
        ui->add_once_box->setValue(all_maps->at(i).add_once);
        ui->row_box->setValue(all_maps->at(i).row);
        ui->colomn_box->setValue(all_maps->at(i).colomn);
        ui->path_cnt_box->setValue(all_maps->at(i).path.size());
    });
    string which_path = "请设置第";
    which_path+=to_string(current_path);
    which_path+="条路径";
    //设置字体大小
    QFont ft;
    ft.setPointSize(14);
    ui->path_infor->setFont(ft);
    ui->path_infor->setText(which_path.c_str());
    //初始化关卡信息，默认设为关卡1的信息
    ui->health_box->setValue(all_maps->at(0).health);
    ui->original_source_box->setValue(all_maps->at(0).original_source);
    ui->speed_box->setValue(all_maps->at(0).source_speed / 1000);
    ui->add_once_box->setValue(all_maps->at(0).add_once);
    ui->row_box->setValue(all_maps->at(0).row);
    ui->colomn_box->setValue(all_maps->at(0).colomn);
    ui->path_cnt_box->setValue(all_maps->at(0).path.size());
    //确认按钮
    connect(ui->confirm_button, &QPushButton::released, this, [=](){
        //如果修改了尺寸，必须修改路径
        if(all_maps->at(map_selected).row != ui->row_box->value() || all_maps->at(map_selected).colomn != ui->colomn_box->value()){
            QMessageBox::information(this, "提示", "修改了原尺寸，原路径已不适用，请修改");
        }
        else if(QMessageBox::Yes == QMessageBox::question(this, "提示", "编辑成功！是否保存？")){
            all_maps->at(map_selected).health = ui->health_box->value();
            all_maps->at(map_selected).original_source = ui->original_source_box->value();
            all_maps->at(map_selected).source_speed = ui->speed_box->value() * 1000;
            all_maps->at(map_selected).add_once = ui->add_once_box->value();
            save();
            ui->MapBox->setCurrentIndex(map_selected);
        }
    });
    //编辑路径按钮
    connect(ui->edit_path, &QPushButton::released, this, [=](){
        all_maps->at(map_selected).health = ui->health_box->value();
        all_maps->at(map_selected).original_source = ui->original_source_box->value();
        all_maps->at(map_selected).source_speed = ui->speed_box->value() * 1000;
        all_maps->at(map_selected).add_once = ui->add_once_box->value();
        all_maps->at(map_selected).row = ui->row_box->value();
        all_maps->at(map_selected).colomn = ui->colomn_box->value();
        this->path_cnt = ui->path_cnt_box->value();
        for(int i = 0; i < all_maps->at(map_selected).colomn ;i++){
            for(int j = 0; j < all_maps->at(map_selected).row ;j++) {
                PathBlock* a_block = new PathBlock(ui->path, i, j);
                a_block->show();
            }
        }
        ui->stackedWidget->setCurrentIndex(1);
    });
    //重新画路径
    connect(ui->reset_button, &QPushButton::released, this, [=](){
        PathBlock::clear_path();
    });
    //保存路径按钮
    connect(ui->confirm_path, &QPushButton::released, this, [=](){
        string which_kind = to_string(ui->path_kind_box->currentIndex() + 1);
        pathes.push_back(which_kind);
        pathes.push_back(PathBlock::output());
        PathBlock::clear_path();
        if(current_path == path_cnt) {
            if(QMessageBox::Yes == QMessageBox::question(this, "提示", "地图编辑成功！是否保存？")){
                //循环添加路径
                all_maps->at(map_selected).path.clear();
                int way_type = 1;
                for(int n = 0; n < pathes.size()/2; n++){
                    vector<coordinate> way;
                    //读取一行，路径种类
                    sscanf(pathes[2*n].c_str(), "%d", &way_type);
                    //读取坐标
                    string a_way = pathes[2*n+1];
                    int i = 0, j = a_way.find(','), cut = a_way.find(' ');
                    int x = 0, y = 0;
                    while(j != string::npos){
                        x = atoi(a_way.substr(i, cut).c_str());
                        y = atoi(a_way.substr(cut+1, j).c_str());
                        coordinate r(x, y);
                        way.push_back(r);
                        i = j+1, j = a_way.find(',', j+1);
                        cut = a_way.find(' ', cut+1);
                    }
                    x = x = atoi(a_way.substr(i, cut).c_str());
                    y = atoi(a_way.substr(cut+1).c_str());
                    coordinate r(x, y);
                    way.push_back(r);
                    Single_Path* path = new Single_Path(way);
                    if(way_type == 1){
                        path->ground = true;
                        //将路径所在处设为 路径种类
                        for(int i = 0; i < way.size(); i++){
                            all_maps->at(map_selected).array[way[i].y * all_maps->at(map_selected).colomn + way[i].x] = way_type;
                        }
                    }
                    else if(way_type == 2){
                        path->ground = false;
                    }
                    all_maps->at(map_selected).path.push_back(*path);
                }
                save();
            }
            ui->stackedWidget->setCurrentIndex(0);
        }
        current_path++;
        string which_path = "请设置第";
        which_path+=to_string(current_path);
        which_path+="条路径";
        ui->path_infor->setText(which_path.c_str());
    });
}

void EditWindow::save(){
    //写入地图文件操作
    ofstream map_file("./Map.txt", ios::out);
    if(!map_file) qDebug()<<"Map文件打开失败";
    //写入说明文字
    map_file << "生命值,初始资源,资源增长速度(ms),一次增加量,行,列" <<endl;
    //写入每张地图
    for(auto map : *all_maps){
        char buffer[100];
        sprintf(buffer, "%d,%d,%d,%d,%d,%d", map.health, map.original_source, map.source_speed, map.add_once, map.row, map.colomn);
        map_file << buffer << endl;
        map_file << to_string(map.path.size()) << endl;
        for(auto single_path : map.path){
            string path_info;
            if(single_path.ground) map_file << "1" << endl;
            else map_file << "2" << endl;
            for(auto coordinate : single_path.way){
                path_info.append(to_string(coordinate.x)).append(" ").append(to_string(coordinate.y)).append(",");
            }
            path_info.pop_back();
            map_file << path_info << endl;
        }
        map_file << "end" << endl;
    }
    map_file.close();
}

void EditWindow::closeEvent(QCloseEvent* event){
    emit on_close();
}

EditWindow::~EditWindow()
{
    delete ui;
}
