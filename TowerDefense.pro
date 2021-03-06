QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    block.cpp \
    defender.cpp \
    enemy.cpp \
    main.cpp \
    mainwindow.cpp \
    map.cpp \
    path.cpp \
    selection.cpp \
    healthbar.cpp \
    waves.cpp \
    start_menue.cpp \
    addwindow.cpp \
    pathblock.cpp \
    editwindow.cpp \
    detail.cpp

HEADERS += \
    block.h \
    defender.h \
    enemy.h \
    mainwindow.h \
    map.h \
    path.h \
    selection.h \
    healthbar.h \
    waves.h \
    start_menue.h \
    addwindow.h \
    pathblock.h \
    editwindow.h \
    detail.h

FORMS += \
    mainwindow.ui \
    addwindow.ui \
    editwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
