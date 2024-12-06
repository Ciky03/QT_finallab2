#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QStyle>
#include <QApplication>
#include <QMessageBox>

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

void MainWindow::on_action_new_triggered()
{
    // TODO: 实现新建事件对话框
    QMessageBox::information(this, tr("新建事件"), tr("即将实现新建事件功能"));
}

void MainWindow::on_action_exit_triggered()
{
    // 隐藏主窗口并保持系统托盘图��
    hide();
}

void MainWindow::on_action_day_view_triggered()
{
    // TODO: 切换到日视图
    statusBar()->showMessage(tr("切换到日视图"), 2000);
}

void MainWindow::on_action_week_view_triggered()
{
    // TODO: 切换到周视图
    statusBar()->showMessage(tr("切换到周视图"), 2000);
}

void MainWindow::on_action_month_view_triggered()
{
    // TODO: 切换到月视图
    statusBar()->showMessage(tr("切换到月视图"), 2000);
}

void MainWindow::on_action_about_triggered()
{
    QMessageBox::about(this, tr("关于日历日程"),
                      tr("日历日程 v1.0\n\n"
                         "一个简单的桌面日历和日程管理工具"));
}

void MainWindow::on_calendarWidget_selectionChanged()
{
    QDate selectedDate = ui->calendarWidget->selectedDate();
    ui->dateLabel->setText(selectedDate.toString("yyyy年MM月dd日"));
    updateEventList();
}

void MainWindow::updateEventList()
{
    ui->eventList->clear();
    // TODO: 从数据库加载选中日期的事件
    // 临时添加一些测试项目
    ui->eventList->addItem(tr("09:00 - 10:00 会议"));
    ui->eventList->addItem(tr("12:00 - 13:00 午餐"));
    ui->eventList->addItem(tr("15:00 - 16:00 项目讨论"));
}
