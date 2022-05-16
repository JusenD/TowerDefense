#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"map.h"
#include"selection.h"
#include<QProgressBar>
#include"waves.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent*);
    Map *the_map;
    Waves* the_waves;
    ~MainWindow();
    void display_source_health();
    void set_select(Defender* selected, QString coming, Selection* selec_selection);
//    void mouseMoveEvent(QMouseEvent* ev);
    bool eventFilter(QObject* object, QEvent* event);
    QString get_coming();
    Defender* get_selected();
    Selection* get_selection();
    void set_spade(QLabel* spade);
    void change_remove(bool whether);
    bool can_remove();
    void enter_call_state(int row, int colomn);
    void cancel_call_state();
    bool is_calling();
private:
    Defender* selected = nullptr;
    QString coming = "";
    Selection* select_selection = nullptr;
    QLabel *spade = nullptr;
    bool remove_now = false;
    Ui::MainWindow *ui;
    int* health;
    QProgressBar* health_remain = nullptr;
    bool calling = false;
    int call_row = 0;
    int call_colomn = 0;
friend void Block::mouseReleaseEvent(QMouseEvent *ev);
friend void Block::enterEvent(QEvent *event);
};
#endif // MAINWINDOW_H
