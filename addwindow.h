#ifndef ADDWINDOW_H
#define ADDWINDOW_H

#include <QWidget>

using namespace std;
namespace Ui {
class AddWindow;
}

class AddWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AddWindow(QWidget *parent = 0);
    ~AddWindow();
signals:
    void on_close();
private:
    Ui::AddWindow *ui;
    int health = 0;
    int original_source = 0;
    int speed = 0;
    int add_once = 0;
    int row = 0;
    int colomn = 0;
    int path_cnt = 0;
    int current_path = 1;
    int path_kind = 1;
    vector<string> pathes;
    void closeEvent(QCloseEvent* event);
};

#endif // ADDWINDOW_H
