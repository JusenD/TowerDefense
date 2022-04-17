#include "mousespace.h"
#include<QMouseEvent>

mouseSpace::mouseSpace(QWidget *parent, QLabel* spade)
    : QWidget{parent}
{
    //创建mouseSpace覆盖
    setParent(parent);
    setFixedSize(parent->width(), parent->height());
    move(0, 0);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setMouseTracking(true);
//    this->installEventFilter(this);
    this->spade = spade;
}

bool mouseSpace::eventFilter(QObject* object, QEvent* event){
    qDebug()<<"mouseSpace再拦截";
    qDebug()<<event->type();
    QMouseEvent* ev = static_cast<QMouseEvent*>(event);
    if(ev->type() == QEvent::MouseMove)

      {
        qDebug()<<"鼠标移动了!";
        if(spade){

        spade->move(ev->x(), ev->y());
        qDebug()<<ev->x()<<ev->y();
        }

      }

      return QWidget::eventFilter(object, event);
}

//void mouseSpace::mouseMoveEvent(QMouseEvent *ev){
//    qDebug()<<"Space鼠标移动了！"<<ev->x()<<ev->y();
//    if(spade){
//        spade->move(ev->x(), ev->y());
//        qDebug()<<ev->x()<<ev->y();
//    }
//}
