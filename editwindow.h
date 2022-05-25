#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QWidget>
#include"map.h"

using namespace std;
namespace Ui {
class EditWindow;
}

class EditWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EditWindow(QWidget *parent, vector<amap>* all_maps);
    ~EditWindow();
signals:
    void on_close();
private:
    Ui::EditWindow *ui;
    int path_cnt = 0;
    int current_path = 1;
    int path_kind = 1;
    vector<string> pathes;
    void closeEvent(QCloseEvent* event);
    vector<amap>* all_maps = nullptr;
    void save();
    int map_selected = 0;
    bool size_change = false;
};

#endif // EDITWINDOW_H
