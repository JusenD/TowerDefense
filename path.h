#ifndef PATH_H
#define PATH_H

#include<vector>

using namespace std;
struct coordinate{
    int x;
    int y;
    coordinate(int x, int y){
        this->x = x;
        this->y = y;
    }
};

class Single_Path
{    
public:
    vector<coordinate> way;
    Single_Path(vector<coordinate> way);
    bool ground = true;
};

#endif // PATH_H
