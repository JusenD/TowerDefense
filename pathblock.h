#ifndef PATHBLOCK_H
#define PATHBLOCK_H

#include <QPushButton>
#include<vector>
using namespace std;
class PathBlock : public QPushButton
{
    Q_OBJECT
    int x= 0;
    int y = 0;
    static int step;
    static int former_x;
    static int former_y;
    static vector<PathBlock*> single_path;
    bool has_selected = false;
public:
    explicit PathBlock(QWidget *parent, int x, int y);
    static string output();
    static void clear_path();
    void mouseReleaseEvent(QMouseEvent* event);
};

#endif // PATHBLOCK_H
