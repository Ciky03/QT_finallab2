#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QStyle>
#include <QApplication>
#include <QMessageBox>
#include <QCalendarWidget>
#include "eventdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , trayIcon(new QSystemTrayIcon(this))
{
    ui->setupUi(this);

    // 设置窗口标题
    setWindowTitle(tr("日历日程"));

    // 设置界面样式
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        QMenuBar {
            background-color: white;
            color: black;
        }
        QMenuBar::item {
            background-color: transparent;
            color: black;
        }
        QMenuBar::item:selected {
            background-color: #e6e6e6;
        }
        QMenuBar::item:pressed {
            background-color: #007bff;
            color: white;
        }
        QCalendarWidget {
            background-color: white;
            color: black;
        }
        QCalendarWidget QTableView {
            background-color: white;
            selection-background-color: #007bff;
            selection-color: white;
        }
        QCalendarWidget QTableView QTableCornerButton::section {
            background-color: white;
        }
        QCalendarWidget QWidget {
            alternate-background-color: #f9f9f9;
            color: black;
        }
        QCalendarWidget QAbstractItemView:enabled {
            color: black;
            background-color: white;
            selection-background-color: #007bff;
            selection-color: white;
        }
        QCalendarWidget QAbstractItemView:disabled {
            color: #808080;
        }
        QCalendarWidget QAbstractItemView::item:hover {
            background-color: #e6e6e6;
        }
        QCalendarWidget QToolButton {
            color: black;
            background-color: transparent;
            border: none;
            border-radius: 4px;
            padding: 4px;
        }
        QCalendarWidget QToolButton:hover {
            background-color: #e6e6e6;
        }
        QCalendarWidget QMenu {
            background-color: white;
            border: 1px solid #ddd;
            color: black;
        }
        QListWidget {
            background-color: white;
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 4px;
            color: black;
        }
        QListWidget::item {
            color: black;
            padding: 4px;
            border-radius: 2px;
        }
        QListWidget::item:hover {
            background-color: #f0f0f0;
        }
        QListWidget::item:selected {
            background-color: #007bff;
            color: white;
        }
        QLabel {
            color: #333;
        }
        QToolBar {
            background-color: white;
            border-bottom: 1px solid #ddd;
            spacing: 2px;
        }
        QToolBar QToolButton {
            color: black;
            border: none;
            border-radius: 4px;
            padding: 4px;
        }
        QToolBar QToolButton:hover {
            background-color: #f0f0f0;
        }
        QToolBar QToolButton::menu-indicator,
        QToolBar QToolButton::menu-button {
            color: black;
        }
        QToolBar QToolButton[popupMode="1"] {
            padding-right: 20px;
            color: black;
        }
        QToolBar * {
            color: black;
        }
        QMenu {
            background-color: white;
            color: black;
        }
        QMenu::item:selected {
            background-color: #007bff;
            color: white;
        }
        QStatusBar {
            background-color: white;
            border-top: 1px solid #ddd;
            color: black;
        }
    )");

    // 设置日历部件的样式
    ui->calendarWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    ui->calendarWidget->setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
    ui->calendarWidget->setNavigationBarVisible(true);
    ui->calendarWidget->setGridVisible(true);

    // 初始化工具栏
    ui->mainToolBar->setIconSize(QSize(24, 24));

    // 如果系统主题没有提供图标，则使用自定义图标
    if (ui->action_new->icon().isNull()) {
        ui->action_new->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    }
    if (ui->action_day_view->icon().isNull()) {
        ui->action_day_view->setIcon(style()->standardIcon(QStyle::SP_FileDialogListView));
    }
    if (ui->action_week_view->icon().isNull()) {
        ui->action_week_view->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    }
    if (ui->action_month_view->icon().isNull()) {
        ui->action_month_view->setIcon(style()->standardIcon(QStyle::SP_FileDialogInfoView));
    }

    // 设置默认图标
    if (ui->action_edit->icon().isNull()) {
        ui->action_edit->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    }
    if (ui->action_delete->icon().isNull()) {
        ui->action_delete->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    }


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
    EventDialog dialog(this);
    QDateTime startTime = ui->calendarWidget->selectedDate().startOfDay();
    dialog.setEventData("", startTime,
                       startTime.addSecs(3600),
                       "", QColor(Qt::blue));
    if (dialog.exec() == QDialog::Accepted) {
        EventItem newEvent;
        newEvent.text = dialog.getEventTitle();
        newEvent.startTime = dialog.getStartTime();
        newEvent.endTime = dialog.getEndTime();
        newEvent.description = dialog.getDescription();
        newEvent.color = dialog.getEventColor();

        // 添加到事件映射中
        QDate eventDate = newEvent.startTime.date();
        eventMap[eventDate].append(newEvent);

        // 更新显示
        updateEventList();
    }
}

void MainWindow::on_action_exit_triggered()
{
    // 隐藏主窗口并保持系统托盘图
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
    QDate selectedDate = ui->calendarWidget->selectedDate();
    
    // 如果选中日期有事件，则显示
    if (eventMap.contains(selectedDate)) {
        const QList<EventItem>& events = eventMap[selectedDate];
        for (const EventItem& event : events) {
            QString eventText = QString("%1 - %2 %3")
                                  .arg(event.startTime.toString("HH:mm"))
                                  .arg(event.endTime.toString("HH:mm"))
                                  .arg(event.text);
            QListWidgetItem *item = new QListWidgetItem(eventText);
            item->setBackground(event.color);
            ui->eventList->addItem(item);
        }
    }
}

void MainWindow::on_action_edit_triggered()
{
    QListWidgetItem *currentItem = ui->eventList->currentItem();
    if (currentItem) {
        EventDialog dialog(this);
        QDate selectedDate = ui->calendarWidget->selectedDate();
        int currentRow = ui->eventList->row(currentItem);
        
        // 获取当前编辑的事件
        EventItem& event = eventMap[selectedDate][currentRow];
        
        dialog.setEventData(event.text,
                          event.startTime,
                          event.endTime,
                          event.description,
                          event.color);
        
        if (dialog.exec() == QDialog::Accepted) {
            // 更新事件数据
            event.text = dialog.getEventTitle();
            event.startTime = dialog.getStartTime();
            event.endTime = dialog.getEndTime();
            event.description = dialog.getDescription();
            event.color = dialog.getEventColor();
            
            // 更新显示
            updateEventList();
        }
    } else {
        QMessageBox::warning(this, tr("提示"),
                           tr("请先选择要编辑的事件"));
    }
}

void MainWindow::on_action_delete_triggered()
{
    QListWidgetItem *currentItem = ui->eventList->currentItem();
    if (currentItem) {
        if (QMessageBox::question(this, tr("确认删除"),
                                tr("是否确定删除事件：%1？").arg(currentItem->text()))
                == QMessageBox::Yes) {
            QDate selectedDate = ui->calendarWidget->selectedDate();
            int currentRow = ui->eventList->row(currentItem);
            
            // 从事件映射中删除
            eventMap[selectedDate].removeAt(currentRow);
            if (eventMap[selectedDate].isEmpty()) {
                eventMap.remove(selectedDate);
            }
            
            // 更新显示
            updateEventList();
        }
    } else {
        QMessageBox::warning(this, tr("提示"),
                           tr("请先选择要删除的事件"));
    }
}

