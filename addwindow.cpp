#include "addwindow.h"
#include "ui_addwindow.h"
#include"pathblock.h"
#include<QDebug>
#include<QMessageBox>
#include<fstream>

AddWindow::AddWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    string which_path = "请设置第";
    which_path+=to_string(current_path);
    which_path+="条路径";
    //设置字体大小
    QFont ft;
    ft.setPointSize(14);
    ui->path_infor->setFont(ft);
    ui->path_infor->setText(which_path.c_str());
    connect(ui->confirm_button, &QPushButton::released, this, [=](){
        this->health = ui->health_box->value();
        this->original_source = ui->original_source_box->value();
        this->speed = ui->speed_box->value() * 1000;
        this->add_once = ui->add_once_box->value();
        this->row = ui->row_box->value();
        this->colomn = ui->colomn_box->value();
        this->path_cnt = ui->path_cnt_box->value();
        for(int i = 0; i < colomn ;i++){
            for(int j = 0; j < row ;j++) {
                PathBlock* a_block = new PathBlock(ui->path, i, j);
                a_block->show();
            }
        }
        if(colomn >= 20){
            this->setFixedWidth(120 + 20 * colomn);
        }
        this->setFixedHeight(200 + 20 * row);
        ui->reset_button->move(this->width()/2 - 50 - 70, this->height() - 70);
        ui->confirm_path->move(this->width()/2 + 50, this->height() - 70);
        ui->stackedWidget->setFixedSize(this->width(), this->height());
        ui->stackedWidget->setCurrentIndex(1);
    });
    connect(ui->reset_button, &QPushButton::released, this, [=](){
        PathBlock::clear_path();
    });
    connect(ui->confirm_path, &QPushButton::released, this, [=](){
        string which_kind = to_string(ui->path_kind_box->currentIndex() + 1);
        pathes.push_back(which_kind);
        pathes.push_back(PathBlock::output());
        qDebug()<<PathBlock::output().c_str();
        PathBlock::clear_path();
        if(current_path == path_cnt) {
            if(QMessageBox::Yes == QMessageBox::question(this, "提示", "地图构建成功！是否保存？")){
                //写入地图文件操作
                ofstream map_file("./Map.txt", ios::app);
                if(!map_file) qDebug()<<"Map文件打开失败";
                char buffer[100];
                sprintf(buffer, "%d,%d,%d,%d,%d,%d", health, original_source, speed, add_once, row, colomn);
                map_file << buffer << endl;
                map_file << to_string(path_cnt) << endl;
                for(auto path : pathes){
                    map_file << path << endl;
                }
                map_file << "end" << endl;
                map_file.close();
            }
            this->close();
        }
        current_path++;
        string which_path = "请设置第";
        which_path+=to_string(current_path);
        which_path+="条路径";
        ui->path_infor->setText(which_path.c_str());
    });
}

void AddWindow::closeEvent(QCloseEvent* event){
    emit on_close();
}

AddWindow::~AddWindow()
{
    delete ui;
}
