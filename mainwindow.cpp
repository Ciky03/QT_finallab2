#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QStyle>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , trayIcon(new QSystemTrayIcon(this))
{
    ui->setupUi(this);
    
    // 设置窗口标题
    setWindowTitle(tr("日历日程"));
    
    // 初始化系统托盘
    trayIcon->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    trayIcon->setToolTip(tr("日历日程"));
    
    // 创建托盘菜单
    QMenu *trayMenu = new QMenu(this);
    trayMenu->addAction(tr("显示主窗口"), this, &QWidget::show);
    trayMenu->addAction(tr("退出"), qApp, &QApplication::quit);
    
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
