#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"map.h"
#include"selection.h"

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
    ~MainWindow();
    void display_source();
    void set_select(Defender* selected, QString coming, Selection* selec_selection);
//    void mouseMoveEvent(QMouseEvent* ev);
    bool eventFilter(QObject* object, QEvent* event);
    QString get_coming();
    Defender* get_selected();
    Selection* get_selection();
    void set_spade(QLabel* spade);
    void change_remove(bool whether);
    bool can_remove();
private:
    Defender* selected = nullptr;
    QString coming = "";
    Selection* select_selection = nullptr;
    QLabel *spade = nullptr;
    bool remove_now = false;
    Ui::MainWindow *ui;
friend void Block::mouseReleaseEvent(QMouseEvent *ev);
friend void Block::enterEvent(QEnterEvent *event);
};
#endif // MAINWINDOW_H
