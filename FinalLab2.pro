QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    eventitemdelegate.cpp \
    eventdialog.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    eventitemdelegate.h \
    eventdialog.h \
    mainwindow.h

FORMS += \
    eventdialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    img/about.png \
    img/day.png \
    img/delete.png \
    img/edit.png \
    img/exit.png \
    img/last.png \
    img/month.png \
    img/new.png \
    img/next.png \
    img/week.png
