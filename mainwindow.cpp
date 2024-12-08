#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QStyle>
#include <QApplication>
#include <QMessageBox>
#include <QCalendarWidget>
#include "eventdialog.h"
#include "eventitemdelegate.h"

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

    // 设置事件列表的代理
    ui->eventList->setItemDelegate(new EventItemDelegate(ui->eventList));
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
    // 如果已经是周视图，就不需要重新创建
    if (weekView && weekView->isVisible()) {
        return;
    }

    currentWeekStart = ui->calendarWidget->selectedDate().addDays(-(ui->calendarWidget->selectedDate().dayOfWeek() - 1));
    setupWeekView();
    statusBar()->showMessage(tr("切换到周视图"), 2000);
}

void MainWindow::on_action_month_view_triggered()
{
    // 如果周视图存在且可见，隐藏它
    if (weekView && weekView->isVisible()) {
        weekView->hide();
        // 显示月视图（日历控件）和右侧部件
        ui->calendarWidget->show();
        ui->rightWidget->show();
        // 更新月视图的事件显示
        updateEventList();
    }
    
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

void MainWindow::setupWeekView()
{
    if (!weekView) {
        weekView = new QWidget(this);
        QVBoxLayout* mainLayout = new QVBoxLayout(weekView);
        
        // 年月显示
        QLabel* yearMonthLabel = new QLabel(weekView);
        yearMonthLabel->setObjectName("yearMonthLabel");
        yearMonthLabel->setAlignment(Qt::AlignLeft);
        mainLayout->addWidget(yearMonthLabel);
        
        // 星期标题和日期显示的容器
        QWidget* calendarContainer = new QWidget(weekView);
        QVBoxLayout* calendarLayout = new QVBoxLayout(calendarContainer);
        calendarLayout->setSpacing(0);  // 减少垂直间距
        calendarLayout->setContentsMargins(0, 0, 0, 0);  // 移除边距

        // 星期标题
        QHBoxLayout* weekDayLayout = new QHBoxLayout();
        weekDayLayout->setSpacing(0);  // 减少水平间距
        weekDayLayout->setContentsMargins(30, 0, 30, 0);  // 左右留出箭头按钮的空间
        QStringList weekDays = {tr("一"), tr("二"), tr("三"), tr("四"), tr("五"), tr("六"), tr("日")};
        for (const QString& day : weekDays) {
            QLabel* label = new QLabel(day, weekView);
            label->setAlignment(Qt::AlignCenter);
            label->setFixedWidth(120);  // 设置固定宽度
            weekDayLayout->addWidget(label);
        }
        calendarLayout->addLayout(weekDayLayout);
        
        // 日期显示区域
        QHBoxLayout* dateLayout = new QHBoxLayout();
        dateLayout->setSpacing(0);  // 减少水平间距
        
        // 向前按钮
        QPushButton* prevBtn = new QPushButton("<", weekView);
        prevBtn->setFixedWidth(30);
        connect(prevBtn, &QPushButton::clicked, this, &MainWindow::showPreviousWeek);
        dateLayout->addWidget(prevBtn);
        
        // 日期显示
        QHBoxLayout* daysLayout = new QHBoxLayout();
        daysLayout->setSpacing(0);  // 减少水平间距
        QButtonGroup* buttonGroup = new QButtonGroup(this);  // 添加按钮组
        for (int i = 0; i < 7; ++i) {
            QPushButton* dayBtn = new QPushButton(weekView);
            dayBtn->setFixedSize(120, 40);
            dayBtn->setCheckable(true);
            buttonGroup->addButton(dayBtn);  // 将按钮添加到按钮组
            connect(dayBtn, &QPushButton::clicked, [this, i]() {
                QDate date = currentWeekStart.addDays(i);
                // 不再更新隐藏的日历控件
                // ui->calendarWidget->setSelectedDate(date);
                // 直接更新事件列表
                updateWeekEvents(date);
                
                // 更新所有按钮的选中状态
                QList<QPushButton*> buttons = weekView->findChildren<QPushButton*>();
                for (int j = 1; j <= 7; ++j) {
                    QDate btnDate = currentWeekStart.addDays(j-1);
                    buttons[j]->setChecked(btnDate == date);
                }
            });
            daysLayout->addWidget(dayBtn);
        }
        dateLayout->addLayout(daysLayout);
        
        // 向后按钮
        QPushButton* nextBtn = new QPushButton(">", weekView);
        nextBtn->setFixedWidth(30);
        connect(nextBtn, &QPushButton::clicked, this, &MainWindow::showNextWeek);
        dateLayout->addWidget(nextBtn);
        
        calendarLayout->addLayout(dateLayout);
        mainLayout->addWidget(calendarContainer);
        
        // 事件列表
        eventListWidget = new QListWidget(weekView);
        mainLayout->addWidget(eventListWidget);
        
        // 设置样式
        weekView->setStyleSheet(R"(
            QWidget {
                color: black;
            }
            QLabel {
                font-size: 14px;
                color: black;
            }
            #yearMonthLabel {
                font-size: 16px;
                font-weight: bold;
                padding: 10px 0;
                color: black;
            }
            QPushButton {
                border: none;
                background: transparent;
                font-size: 14px;
                color: black;
            }
            QPushButton:checked {
                background: #007bff;
                color: white;
                border-radius: 5px;
            }
            QPushButton:hover:!checked {
                background: #f0f0f0;
                color: black;
                border-radius: 5px;
            }
            QListWidget {
                color: black;
                font-size: 13px;
            }
            QListWidget::item {
                padding: 8px;
                border-bottom: 1px solid #eee;
                min-height: 30px;
            }
            QListWidget::item:selected {
                background: #f5f5f5;
                color: black;
            }
        )");

        // 不再使用setCentralWidget，而是隐藏原中央部件并显示weekView
        weekView->setParent(centralWidget());
        weekView->setGeometry(centralWidget()->rect());
    }
    
    weekView->show();
    ui->calendarWidget->hide();
    ui->rightWidget->hide();  // 隐藏右侧部件
    updateWeekView();
}

void MainWindow::updateWeekView()
{
    if (!weekView) return;
    
    // 更新年月标签
    QLabel* yearMonthLabel = weekView->findChild<QLabel*>("yearMonthLabel");
    if (yearMonthLabel) {
        yearMonthLabel->setText(QString("%1年%2月").arg(currentWeekStart.year())
                                                 .arg(currentWeekStart.month()));
    }
    
    // 更新日期按钮
    QList<QPushButton*> dayButtons = weekView->findChildren<QPushButton*>();
    QDate selectedDate = currentWeekStart; // 默认选中第一天
    // 跳过前后箭头按钮
    for (int i = 1; i <= 7; ++i) {
        QPushButton* btn = dayButtons[i];
        QDate date = currentWeekStart.addDays(i-1);
        btn->setText(QString::number(date.day()));
        if (i == 1) {
            btn->setChecked(true);
            selectedDate = date;
        } else {
            btn->setChecked(false);
        }
    }
    
    // 更新事件列表
    updateWeekEvents(selectedDate);
}

void MainWindow::updateWeekEvents(const QDate& date)
{
    eventListWidget->clear();
    eventListWidget->setSelectionMode(QAbstractItemView::SingleSelection);  // 确保单选模式
    
    if (eventMap.contains(date)) {
        int eventIndex = 0;  // 用于跟踪事件索引
        for (const EventItem& event : eventMap[date]) {
            // 为每个事件创建一个容器widget
            QWidget* eventWidget = new QWidget;
            QVBoxLayout* eventLayout = new QVBoxLayout(eventWidget);
            eventLayout->setSpacing(0);
            eventLayout->setContentsMargins(0, 0, 0, 0);
            
            // 添加事件标题（第一行）
            QWidget* titleContainer = new QWidget;
            QHBoxLayout* titleLayout = new QHBoxLayout(titleContainer);
            titleLayout->setSpacing(8);
            titleLayout->setContentsMargins(8, 8, 8, 8);

            // 添加颜色指示器
            QLabel* colorIndicator = new QLabel;
            colorIndicator->setFixedSize(12, 12);
            colorIndicator->setStyleSheet(QString("background-color: %1; border-radius: 6px;")
                                        .arg(event.color.name()));
            titleLayout->addWidget(colorIndicator);

            // 添加时间和标题
            QLabel* titleLabel = new QLabel;
            QString timeText = QString("%1 - %2")
                                .arg(event.startTime.toString("HH:mm"))
                                .arg(event.endTime.toString("HH:mm"));
            titleLabel->setStyleSheet("color: #666666; font-size: 14px; background: transparent;");
            titleLabel->setText(timeText + " " + event.text);
            titleLabel->setMinimumHeight(30);
            titleLayout->addWidget(titleLabel, 1);  // 添加拉伸因子1

            eventLayout->addWidget(titleContainer);
            
            // 添加事件描述（第二行）
            if (!event.description.isEmpty()) {
                QLabel* descLabel = new QLabel;
                descLabel->setText(event.description);
                descLabel->setStyleSheet(
                    "background-color: #F0F7FF;"
                    "border-radius: 6px;"
                    "padding: 8px;"
                    "margin: 4px 8px;"
                );
                descLabel->setWordWrap(true);
                descLabel->setMinimumHeight(50);  // 设置最小高度
                descLabel->adjustSize();  // 自动调整大小以适应内容
                eventLayout->addWidget(descLabel);
            }
            
            // 创建列表项并设置widget
            QListWidgetItem* eventItem = new QListWidgetItem;
            eventItem->setData(Qt::UserRole, eventIndex);  // 存储事件索引
            eventWidget->adjustSize();  // 调整容器大小
            eventItem->setSizeHint(eventWidget->sizeHint());  // 使用容器的建议大小
            eventListWidget->addItem(eventItem);
            eventListWidget->setItemWidget(eventItem, eventWidget);
            
            eventIndex++;
        }
        
        // 连接选择改变信号
        connect(eventListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
            int eventIndex = item->data(Qt::UserRole).toInt();
            // 选中整个事件（所有相关项）
            for (int i = 0; i < eventListWidget->count(); ++i) {
                QListWidgetItem* currentItem = eventListWidget->item(i);
                if (currentItem->data(Qt::UserRole).toInt() == eventIndex) {
                    currentItem->setSelected(true);
                    // 设置选中状态的背景色
                    QWidget* widget = eventListWidget->itemWidget(currentItem);
                    if (widget) {
                        widget->setStyleSheet("background-color: #f0f0f0;");
                    }
                } else {
                    currentItem->setSelected(false);
                    QWidget* widget = eventListWidget->itemWidget(currentItem);
                    if (widget) {
                        widget->setStyleSheet("");
                    }
                }
            }
        });
    }
}

void MainWindow::showPreviousWeek()
{
    currentWeekStart = currentWeekStart.addDays(-7);
    updateWeekView();
}

void MainWindow::showNextWeek()
{
    currentWeekStart = currentWeekStart.addDays(7);
    updateWeekView();
}

