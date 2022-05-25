#include "pathblock.h"

int PathBlock::step = 0;
int PathBlock::former_x = 0;
int PathBlock::former_y = 0;
vector<PathBlock*> PathBlock::single_path;
PathBlock::PathBlock(QWidget *parent, int x, int y) : QPushButton(parent)
{
    this->x = x;
    this->y = y;
    this->setFixedSize(18, 18);
    this->move(60 + x*20, 90 + y*20);
}

void PathBlock::mouseReleaseEvent(QMouseEvent* event){
    bool ok = false;
    if(!step) ok = true;
    else if(abs(x - former_x) + abs(y - former_y) <= 1) ok = true;
    if(ok && !has_selected){
        has_selected = true;
        step++;
        former_x = x;
        former_y = y;
        this->setStyleSheet("background-color:blue");
        single_path.push_back(this);
    }
}

void PathBlock::clear_path(){
    step = 0;
    for(auto block : single_path) {
        block->has_selected = false;
        block->setStyleSheet("background-color:white");
    }
    single_path.clear();

}

string PathBlock::output(){
    string sequence;
    for(auto coordinate : single_path){
        string buffer = to_string(coordinate->x);
        buffer.append(" ").append(to_string(coordinate->y)).append(",");
        sequence += buffer;
    }
    sequence = sequence.substr(0, sequence.length() - 1);
    return sequence;
}
