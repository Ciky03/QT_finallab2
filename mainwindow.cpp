#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QStyle>
#include <QApplication>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QToolButton>
#include <QTableView>
#include<QTimeEdit>
#include<QScrollArea>
#include "eventdialog.h"
#include "eventitemdelegate.h"
#include "idatabase.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , trayIcon(new QSystemTrayIcon(this))
    , database(new IDatabase(this))
{
    ui->setupUi(this);

    // 初始化当前日期为今天
    currentDate = QDate::currentDate();

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

    // 设置日历导航按钮的式
    QToolButton* prevMonth = ui->calendarWidget->findChild<QToolButton *>("qt_calendar_prevmonth");
    QToolButton* nextMonth = ui->calendarWidget->findChild<QToolButton *>("qt_calendar_nextmonth");

    if (prevMonth) {
        prevMonth->setIcon(QIcon(":/img/img/last.png"));
        prevMonth->setIconSize(QSize(16, 16));
    }
    if (nextMonth) {
        nextMonth->setIcon(QIcon(":/img/img/next.png"));
        nextMonth->setIconSize(QSize(16, 16));
    }

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

    // 设置拖放
    setAcceptDrops(true);
    ui->eventList->setDragDropMode(QAbstractItemView::DragOnly);
    ui->calendarWidget->setAcceptDrops(true);

    // 安装事件过滤器
    installEventFilter(this);

    // 为日历控件安装事件过滤器
    ui->calendarWidget->installEventFilter(this);

    // 连接日历页面改变信号
    connect(ui->calendarWidget, &QCalendarWidget::currentPageChanged,
            this, &MainWindow::updateEventList);

    // 连接双击信号
    connect(ui->eventList, &QListWidget::itemDoubleClicked,
            this, &MainWindow::onEventItemDoubleClicked);

    // 初始化数据库并加载事件
    if (!database->initDatabase()) {
        QMessageBox::warning(this, tr("错误"), tr("数据库初始化失败"));
    } else {
        loadEventsFromDatabase();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_new_triggered()
{
    QDate selectedDate;
    if (dayView && dayView->isVisible()) {
        // 在日视图中，使用当前显示的日期
        selectedDate = currentDate;
    } else if (weekView && weekView->isVisible()) {
        // 在周视图中，获取当前选中的日期
        QList<QPushButton *> buttons = weekView->findChildren<QPushButton *>();
        for (int i = 1; i <= 7; ++i) {
            if (buttons[i]->isChecked()) {
                selectedDate = currentWeekStart.addDays(i - 1);
                break;
            }
        }
    } else {
        // 在月视图中，使用日历控件的选中日期
        selectedDate = ui->calendarWidget->selectedDate();
    }

    EventDialog dialog(this);
    QDateTime startTime = selectedDate.startOfDay();
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

        // 保存到数据库并获取新的事件ID
        int newId = saveEventToDatabase(newEvent);
        if (newId > 0) {
            newEvent.id = newId;  // 设置事件ID
            
            // 添加事件到映射中
            QDate eventDate = newEvent.startTime.date();
            eventMap[eventDate].append(newEvent);

            // 根据当前视图更新显示
            if (dayView && dayView->isVisible()) {
                updateDayView();
            } else if (weekView && weekView->isVisible()) {
                updateWeekView();
                updateWeekEvents(selectedDate);
            } else {
                updateEventList();
            }
        }
    }
}

void MainWindow::on_action_exit_triggered()
{
    // 隐藏主窗口并保持系统托盘图
    hide();
}

void MainWindow::on_action_day_view_triggered()
{
    // 如果已经是日视图，就不需要重新创建
    if (dayView && dayView->isVisible()) {
        return;
    }

    // 隐藏周视图
    if (weekView) {
        weekView->hide();
    }

    // 如果当前日期无效，使用当前选中的日期
    if (!currentDate.isValid()) {
        currentDate = ui->calendarWidget->selectedDate();
    }

    // 创建日视图
    setupDayView();

    // 更新状态栏
    statusBar()->showMessage(tr("切换到日视图"), 2000);
}

void MainWindow::on_action_week_view_triggered()
{
    // 如果已经是周视图，就不需要重新创建
    if (weekView && weekView->isVisible()) {
        return;
    }

    // 隐藏日视图
    if (dayView) {
        dayView->hide();
    }

    currentWeekStart = ui->calendarWidget->selectedDate().addDays(-(ui->calendarWidget->selectedDate().dayOfWeek() - 1));
    setupWeekView();
    statusBar()->showMessage(tr("切换到周视图"), 2000);
}

void MainWindow::on_action_month_view_triggered()
{
    // 隐藏周视图和日视图
    if (weekView) {
        weekView->hide();
    }
    if (dayView) {
        dayView->hide();
    }

    // 显示月视图（日历控件）和右侧部件
    ui->calendarWidget->show();
    ui->rightWidget->show();

    // 更新月视图的事件显示
    updateEventList();

    statusBar()->showMessage(tr("切换到月视图"), 2000);
}

void MainWindow::on_action_about_triggered()
{
    QMessageBox::about(this, tr("关于日历日程控"),
                       tr("2022441010234-张创阳-桌面日历日程控件"
                         ));
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

    // 如果选中日有事件，则显示在列表中
    if (eventMap.contains(selectedDate)) {
        const QList<EventItem> &events = eventMap[selectedDate];
        for (int i = 0; i < events.size(); ++i) {
            const EventItem& event = events[i];
            QString eventText = QString("%1 - %2 %3")
                                .arg(event.startTime.toString("HH:mm"))
                                .arg(event.endTime.toString("HH:mm"))
                                .arg(event.text);
            QListWidgetItem *item = new QListWidgetItem(eventText);
            item->setBackground(event.color);
            item->setData(Qt::UserRole, i);  // 存储事件索引
            ui->eventList->addItem(item);
        }
    }

    // 更新日历中的事件指示器
    QCalendarWidget* calendar = ui->calendarWidget;
    QTableView* calendarView = calendar->findChild<QTableView *>();
    if (!calendarView) return;

    // 清理旧的指示器
    QList<QWidget *> oldDots = calendar->findChildren<QWidget *>("dotsContainer");
    for (QWidget * widget : oldDots) {
        delete widget;
    }

    // 获取当前显示的月份的第一天
    QDate firstOfMonth(calendar->yearShown(), calendar->monthShown(), 1);
    int firstDayOfWeek = firstOfMonth.dayOfWeek();  // 1=周一，7=周日

    // 历当前月份的所有日期
    for (int day = 1; day <= firstOfMonth.daysInMonth(); ++day) {
        QDate date(firstOfMonth.year(), firstOfMonth.month(), day);
        if (eventMap.contains(date)) {
            // 计算日期在表格中的行列位置
            int firstDayOffset = firstOfMonth.dayOfWeek() - 1;  // 月初的偏移（0=周一）
            int totalDays = day + firstDayOffset - 1;  // 从月初开始的总天数
            int row = totalDays / 7;  // 行号
            int col = totalDays % 7;  // 列号

            // 获取日期单元格的矩形区域
            QModelIndex index = calendarView->model()->index(row, col);
            QRect cellRect = calendarView->visualRect(index);

            // 创建事件指示器容器
            QWidget* dotsContainer = new QWidget(calendarView->viewport());
            dotsContainer->setObjectName("dotsContainer");
            QHBoxLayout* dotsLayout = new QHBoxLayout(dotsContainer);
            dotsLayout->setSpacing(2);
            dotsLayout->setContentsMargins(0, 0, 0, 0);
            dotsLayout->setAlignment(Qt::AlignCenter);

            // 添加事件指示器
            for (const EventItem& event : eventMap[date]) {
                QLabel* dot = new QLabel(dotsContainer);
                dot->setFixedSize(4, 4);  // 小圆点大小
                dot->setStyleSheet(QString("background-color: %1; border-radius: 2px;")
                                   .arg(event.color.name()));
                dotsLayout->addWidget(dot);
            }

            // 设置指示器位置（在日期下方）
            int dotsHeight = 6;  // 圆容器的高度
            dotsContainer->setFixedSize(cellRect.width(), dotsHeight);

            // 将指示器位置移到单元格底部，但稍微往上一点
            int yOffset = cellRect.height() * 1.7;  // 从2.0改为1.7，向上移动0.3个单元格高度
            dotsContainer->setGeometry(cellRect.x(),
                                       cellRect.y() + yOffset,  // 使用新的y偏移
                                       cellRect.width(),
                                       dotsHeight);
            dotsContainer->show();

            // 将容器设置为视口的子部件，这样会跟随滚动
            dotsContainer->setParent(calendarView->viewport());
        }
    }

    // 启用拖放
    ui->eventList->setDragEnabled(true);
    ui->eventList->setDragDropMode(QAbstractItemView::DragOnly);
    ui->calendarWidget->setAcceptDrops(true);
}

void MainWindow::on_action_edit_triggered()
{
    if (dayView && dayView->isVisible()) {
        // 日视图编辑
        QListWidget* detailsList = dayView->findChild<QListWidget *>("dayViewDetailsList");
        if (!detailsList || detailsList->count() == 0) {
            QMessageBox::warning(this, tr("提示"), tr("请先选择要编辑的事件"));
            return;
        }

        // 获取事件标题
        QWidget* detailWidget = detailsList->itemWidget(detailsList->item(0));
        if (!detailWidget) return;

        QList<QLabel *> labels = detailWidget->findChildren<QLabel *>();
        QString eventTitle;
        for (QLabel * label : labels) {
            if (label->styleSheet().contains("font-weight: bold")) {
                eventTitle = label->text();
                break;
            }
        }

        if (eventTitle.isEmpty()) return;

        // 查找并编辑事件
        QList<EventItem> &events = eventMap[currentDate];
        for (EventItem& event : events) {
            if (event.text == eventTitle) {
                EventDialog dialog(this);
                dialog.setEventData(event.text, event.startTime, event.endTime, event.description, event.color);
                if (dialog.exec() == QDialog::Accepted) {
                    // 更新事件信息
                    event.text = dialog.getEventTitle();
                    event.startTime = dialog.getStartTime();
                    event.endTime = dialog.getEndTime();
                    event.description = dialog.getDescription();
                    event.color = dialog.getEventColor();

                    // 更新数据库
                    updateEventInDatabase(event.id, event);

                    // 更新日视图
                    updateDayView();
                }
                break;
            }
        }
    } else if (weekView && weekView->isVisible()) {
        // 周视图编辑
        QListWidgetItem* currentItem = eventListWidget->currentItem();
        if (!currentItem) {
            QMessageBox::warning(this, tr("提示"), tr("请先选择要编辑的事件"));
            return;
        }

        QList<QPushButton *> buttons = weekView->findChildren<QPushButton *>();
        QDate selectedDate;
        for (int i = 1; i <= 7; ++i) {
            if (buttons[i]->isChecked()) {
                selectedDate = currentWeekStart.addDays(i - 1);
                break;
            }
        }

        int eventIndex = currentItem->data(Qt::UserRole).toInt();
        if (eventMap.contains(selectedDate) && eventIndex < eventMap[selectedDate].size()) {
            EventItem& event = eventMap[selectedDate][eventIndex];
            EventDialog dialog(this);
            dialog.setEventData(event.text, event.startTime, event.endTime, event.description, event.color);
            if (dialog.exec() == QDialog::Accepted) {
                // 更新事件信息
                event.text = dialog.getEventTitle();
                event.startTime = dialog.getStartTime();
                event.endTime = dialog.getEndTime();
                event.description = dialog.getDescription();
                event.color = dialog.getEventColor();

                // 更新数据库
                updateEventInDatabase(event.id, event);

                // 更新视图
                updateWeekView();
                updateWeekEvents(selectedDate);
            }
        }
    } else {
        // 月视图编辑
        QListWidgetItem* currentItem = ui->eventList->currentItem();
        if (!currentItem) {
            QMessageBox::warning(this, tr("提示"), tr("请先选择要编辑的事件"));
            return;
        }

        QDate selectedDate = ui->calendarWidget->selectedDate();
        int currentRow = ui->eventList->row(currentItem);
        
        if (eventMap.contains(selectedDate) && currentRow < eventMap[selectedDate].size()) {
            EventItem& event = eventMap[selectedDate][currentRow];
            EventDialog dialog(this);
            dialog.setEventData(event.text, event.startTime, event.endTime, event.description, event.color);
            if (dialog.exec() == QDialog::Accepted) {
                // 更新事件信息
                event.text = dialog.getEventTitle();
                event.startTime = dialog.getStartTime();
                event.endTime = dialog.getEndTime();
                event.description = dialog.getDescription();
                event.color = dialog.getEventColor();

                // 更新数据库
                updateEventInDatabase(event.id, event);

                // 更新视图
                updateEventList();
            }
        }
    }
}

void MainWindow::on_action_delete_triggered()
{
    if (dayView && dayView->isVisible()) {
        // 日视图删除
        QListWidget* detailsList = dayView->findChild<QListWidget *>("dayViewDetailsList");
        if (!detailsList || detailsList->count() == 0) {
            QMessageBox::warning(this, tr("提示"), tr("请先选择要删除的事件"));
            return;
        }

        // 获取事件标题
        QWidget* detailWidget = detailsList->itemWidget(detailsList->item(0));
        if (!detailWidget) return;

        QList<QLabel *> labels = detailWidget->findChildren<QLabel *>();
        QString eventTitle;
        for (QLabel * label : labels) {
            // 修改标题获取逻辑
            if (label->styleSheet().contains("font-weight: bold")) {
                eventTitle = label->text();
                break;
            }
        }

        if (eventTitle.isEmpty()) return;

        // 查找并删除事件
        QList<EventItem> &events = eventMap[currentDate];
        for (int i = 0; i < events.size(); ++i) {
            if (events[i].text == eventTitle) {
                if (QMessageBox::question(this, tr("确认删除"),
                                          tr("是否确定删除事���：%1？").arg(eventTitle)) == QMessageBox::Yes) {
                    // 从数据库中删除
                    deleteEventFromDatabase(events[i].id);
                    
                    // 从内存中删除
                    events.removeAt(i);
                    if (events.isEmpty()) {
                        eventMap.remove(currentDate);
                    }
                    updateDayView();
                    detailsList->clear();
                }
                break;
            }
        }
    } else if (weekView && weekView->isVisible()) {
        // 周视图删除
        QListWidgetItem* currentItem = eventListWidget->currentItem();
        if (!currentItem) {
            QMessageBox::warning(this, tr("提示"), tr("请先选择要删除的事件"));
            return;
        }

        QList<QPushButton *> buttons = weekView->findChildren<QPushButton *>();
        QDate selectedDate;
        for (int i = 1; i <= 7; ++i) {
            if (buttons[i]->isChecked()) {
                selectedDate = currentWeekStart.addDays(i - 1);
                break;
            }
        }

        int eventIndex = currentItem->data(Qt::UserRole).toInt();
        if (eventMap.contains(selectedDate) && eventIndex < eventMap[selectedDate].size()) {
            QString eventText = eventMap[selectedDate][eventIndex].text;
            if (QMessageBox::question(this, tr("确认删除"),
                                      tr("是否确定删除事件：%1？").arg(eventText)) == QMessageBox::Yes) {
                // 从数据库中删除
                deleteEventFromDatabase(eventMap[selectedDate][eventIndex].id);
                
                // 从内存中删除
                eventMap[selectedDate].removeAt(eventIndex);
                if (eventMap[selectedDate].isEmpty()) {
                    eventMap.remove(selectedDate);
                }
                updateWeekView();
                updateWeekEvents(selectedDate);
            }
        }
    } else {
        // 月视图删除
        QListWidgetItem* currentItem = ui->eventList->currentItem();
        if (!currentItem) {
            QMessageBox::warning(this, tr("提示"), tr("请先选择要删除的事件"));
            return;
        }

        QDate selectedDate = ui->calendarWidget->selectedDate();
        int currentRow = ui->eventList->row(currentItem);
        QString eventText = eventMap[selectedDate][currentRow].text;

        if (QMessageBox::question(this, tr("确认删除"),
                                  tr("是否确定删除事件：%1？").arg(eventText)) == QMessageBox::Yes) {
            // 从数据库中删除
            deleteEventFromDatabase(eventMap[selectedDate][currentRow].id);
            
            // 从内存中删除
            eventMap[selectedDate].removeAt(currentRow);
            if (eventMap[selectedDate].isEmpty()) {
                eventMap.remove(selectedDate);
            }
            updateEventList();
        }
    }
}

void MainWindow::setupWeekView()
{
    // 如果weekView已经存在，先删除它
    if (weekView) {
        delete weekView;
        weekView = nullptr;
    }

    // 创建新的weekView
    weekView = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(weekView);

    // 月显示
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
        label->setFixedWidth(120);  // 设置固定度
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

    // 期显示
    QHBoxLayout* daysLayout = new QHBoxLayout();
    daysLayout->setSpacing(0);  // 少水平间距
    QButtonGroup* buttonGroup = new QButtonGroup(this);  // 添加按钮组
    for (int i = 0; i < 7; ++i) {
        QPushButton* dayBtn = new QPushButton(weekView);
        dayBtn->setFixedSize(120, 40);
        dayBtn->setCheckable(true);
        buttonGroup->addButton(dayBtn);  // 将按钮添加到按钮组
        connect(dayBtn, &QPushButton::clicked, [this, i]() {
            QDate date = currentWeekStart.addDays(i);
            // 不再更隐藏日件
            // ui->calendarWidget->setSelectedDate(date);
            // 直接事件列表
            updateWeekEvents(date);

            // 更新所有按钮的选中状态
            QList<QPushButton *> buttons = weekView->findChildren<QPushButton *>();
            for (int j = 1; j <= 7; ++j) {
                QDate btnDate = currentWeekStart.addDays(j - 1);
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

    // 不再使用setCentralWidget，而是隐藏原央部件并显示weekView
    weekView->setParent(centralWidget());
    weekView->setGeometry(centralWidget()->rect());

    // 添加这些行来显示和更新周视图
    weekView->show();
    ui->calendarWidget->hide();
    ui->rightWidget->hide();  // 藏右侧部件
    updateWeekView();
}

void MainWindow::updateWeekView()
{
    if (!weekView) return;

    try {
        // 更新月标签
        QLabel* yearMonthLabel = weekView->findChild<QLabel *>("yearMonthLabel");
        if (yearMonthLabel) {
            yearMonthLabel->setText(QString("%1年%2月").arg(currentWeekStart.year())
                                    .arg(currentWeekStart.month()));
        }

        // 更新日期按钮
        QList<QPushButton *> buttons = weekView->findChildren<QPushButton *>();
        if (buttons.size() < 8) return;  // 确保有足够的按钮（包括前后箭头）

        // 认选中第一天
        QDate selectedDate = currentWeekStart;

        // 跳过前后箭头按钮
        for (int i = 1; i <= 7; ++i) {
            QPushButton* btn = buttons[i];
            if (!btn) continue;  // 检查按钮是否有效

            QDate date = currentWeekStart.addDays(i - 1);

            // 清理按钮上的所有事件指示器
            QList<QWidget *> oldWidgets = btn->findChildren<QWidget *>("dotsContainer");
            for (QWidget * widget : oldWidgets) {
                if (widget) {
                    widget->setParent(nullptr);
                    widget->deleteLater();
                }
            }

            // 设置日期文本
            btn->setText(QString::number(date.day()));

            // 如果该日期有事件，添加事件指示器
            if (eventMap.contains(date)) {
                QWidget* dotsContainer = new QWidget(btn);
                dotsContainer->setObjectName("dotsContainer");
                QHBoxLayout* dotsLayout = new QHBoxLayout(dotsContainer);
                dotsLayout->setSpacing(2);
                dotsLayout->setContentsMargins(0, 0, 0, 0);
                dotsLayout->setAlignment(Qt::AlignCenter);

                for (const EventItem& event : eventMap[date]) {
                    QLabel* dot = new QLabel(dotsContainer);
                    dot->setFixedSize(6, 6);
                    dot->setStyleSheet(QString("background-color: %1; border-radius: 3px;")
                                       .arg(event.color.name()));
                    dotsLayout->addWidget(dot);
                }

                // 设置事件指示器的位置在按钮底部
                dotsContainer->setGeometry(0, btn->height() - 15, btn->width(), 12);
                dotsContainer->show();
            }

            // 设置选中状态
            btn->setChecked(i == 1);
            if (i == 1) {
                selectedDate = date;
            }
        }

        // 更新事件列表
        if (eventListWidget) {
            eventListWidget->clear();
            updateWeekEvents(selectedDate);
        }

    } catch (const std::exception& e) {
        qDebug() << "Error in updateWeekView:" << e.what();
    }
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

            // 加颜色指示器
            QLabel* colorIndicator = new QLabel;
            colorIndicator->setFixedSize(12, 12);
            colorIndicator->setStyleSheet(QString("background-color: %1; border-radius: 6px;")
                                          .arg(event.color.name()));
            titleLayout->addWidget(colorIndicator);

            // 添加时间标题
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
                descLabel->setMinimumHeight(50);  // 设置最小度
                descLabel->adjustSize();  // 自动调整大小以适应内容
                eventLayout->addWidget(descLabel);
            }

            // 创建表项并设置widget
            QListWidgetItem* eventItem = new QListWidgetItem;
            eventItem->setData(Qt::UserRole, eventIndex);  // 存储事件索引
            eventWidget->adjustSize();  // 整容器大小
            eventItem->setSizeHint(eventWidget->sizeHint());  // 使器的建大小
            eventListWidget->addItem(eventItem);
            eventListWidget->setItemWidget(eventItem, eventWidget);

            eventIndex++;
        }

        // 连接选择改变信号
        connect(eventListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem * item) {
            int eventIndex = item->data(Qt::UserRole).toInt();
            // 选中整事件（所有相关）
            for (int i = 0; i < eventListWidget->count(); ++i) {
                QListWidgetItem* currentItem = eventListWidget->item(i);
                if (currentItem->data(Qt::UserRole).toInt() == eventIndex) {
                    currentItem->setSelected(true);
                    // 设置选中状的背景色
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

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* event)
{
    // 获取拖放的目标日期
    QCalendarWidget* calendar = ui->calendarWidget;
    QPoint pos = calendar->mapFromGlobal(QCursor::pos());

    // 算标位置对应的日期
    QDate dropDate = calendar->selectedDate();  // 认使用选中日期

    // 尝试根据位置计算日期
    if (calendar->rect().contains(pos)) {
        int row = pos.y() / (calendar->height() / 6);  // 6行
        int col = pos.x() / (calendar->width() / 7);   // 7

        QDate firstDay = QDate(calendar->selectedDate().year(),
                               calendar->selectedDate().month(), 1);
        int offset = firstDay.dayOfWeek() - 1;  // 计算月初是星期几

        int day = row * 7 + col - offset + 1;
        QDate calculatedDate = firstDay.addDays(day - 1);

        // 确保日期在当前月份内
        if (calculatedDate.month() == calendar->selectedDate().month()) {
            dropDate = calculatedDate;
        }
    }

    // 获取原始事件的信息
    QDate originalDate = calendar->selectedDate();
    int eventIndex = ui->eventList->currentItem()->data(Qt::UserRole).toInt();

    // 如果日期不同，则移动事件
    if (dropDate != originalDate) {
        handleEventDrop(dropDate, eventIndex);
    }

    event->acceptProposedAction();
}

void MainWindow::handleEventDrop(const QDate& newDate, int eventIndex)
{
    QDate oldDate = ui->calendarWidget->selectedDate();

    if (!eventMap.contains(oldDate) || eventIndex >= eventMap[oldDate].size()) {
        return;
    }

    // 获取事件
    EventItem event = eventMap[oldDate][eventIndex];

    // 调整事件的日期
    QTime startTime = event.startTime.time();
    QTime endTime = event.endTime.time();
    event.startTime = QDateTime(newDate, startTime);
    event.endTime = QDateTime(newDate, endTime);

    // 更新数据库
    updateEventInDatabase(event.id, event);

    // 从旧日期移除事件
    eventMap[oldDate].removeAt(eventIndex);
    if (eventMap[oldDate].isEmpty()) {
        eventMap.remove(oldDate);
    }

    // 添加到新日期
    eventMap[newDate].append(event);

    // 更新显示
    ui->calendarWidget->setSelectedDate(newDate);
    updateEventList();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->calendarWidget) {
        if (event->type() == QEvent::DragEnter) {
            QDragEnterEvent *dragEvent = static_cast<QDragEnterEvent*>(event);
            dragEvent->acceptProposedAction();
            return true;
        }
        else if (event->type() == QEvent::DragMove) {
            QDragMoveEvent *moveEvent = static_cast<QDragMoveEvent*>(event);
            moveEvent->acceptProposedAction();
            return true;
        }
        else if (event->type() == QEvent::Drop) {
            QDropEvent *dropEvent = static_cast<QDropEvent*>(event);
            QPoint pos = dropEvent->pos();
            
            // 算鼠标位置对应的日期
            QCalendarWidget* calendar = ui->calendarWidget;
            
            // 获取日历视图的大小信息
            int totalHeight = calendar->height();
            int totalWidth = calendar->width();
            int headerHeight = totalHeight / 8;  // 估计标题栏高度
            int cellHeight = (totalHeight - headerHeight) / 6;
            int cellWidth = totalWidth / 7;
            
            // 计算行列位置
            int row = (pos.y() - headerHeight) / cellHeight;
            int col = pos.x() / cellWidth;
            
            // 确保行列在有效范围内
            if (row >= 0 && row < 6 && col >= 0 && col < 7) {
                // 获取当前显示的月份的第一天
                QDate firstOfMonth(calendar->yearShown(), calendar->monthShown(), 1);
                
                // 计算日历第一个格子对应的日期（可能是上个月的日期）
                QDate firstDate = firstOfMonth.addDays(-(firstOfMonth.dayOfWeek() - 1));
                
                // 计算目标日期
                QDate targetDate = firstDate.addDays(row * 7 + col);
                
                // 获取原始事件的信���
                QDate originalDate = calendar->selectedDate();
                int eventIndex = ui->eventList->currentItem()->data(Qt::UserRole).toInt();
                
                // 如果日期不同，则移动事件
                if (targetDate != originalDate) {
                    handleEventDrop(targetDate, eventIndex);
                }
            }
            
            dropEvent->acceptProposedAction();
            return true;
        }
    }
    else if (dayView && dayView->isVisible()) {
        // 处理日视图的事件点击
        if (event->type() == QEvent::MouseButtonPress) {
            QWidget* widget = qobject_cast<QWidget*>(obj);
            if (widget && widget->parent()) {
                QWidget* parentWidget = qobject_cast<QWidget*>(widget->parent());
                if (parentWidget && parentWidget->property("timeSlot").isValid()) {
                    int slotHour = parentWidget->property("timeSlot").toInt();

                    // 获取对应时间段的事件
                    if (eventMap.contains(currentDate)) {
                        const QList<EventItem> &events = eventMap[currentDate];

                        // 获取点击的事件部在布局中的位置
                        QHBoxLayout* slotLayout = qobject_cast<QHBoxLayout *>(parentWidget->layout());
                        if (!slotLayout) return false;

                        int clickedIndex = slotLayout->indexOf(widget);
                        if (clickedIndex < 0) return false;

                        // 找到对应时间段内的第clickedIndex个事件
                        int foundEventIndex = -1;
                        for (int i = 0; i < events.size(); ++i) {
                            const EventItem& event = events[i];
                            int eventStartHour = event.startTime.time().hour();
                            if (slotHour <= eventStartHour && slotHour + 2 > eventStartHour) {
                                foundEventIndex++;
                                if (foundEventIndex == clickedIndex) {
                                    // 找到了点击的事件，更新右侧详情显示
                                    QListWidget* detailsList = dayView->findChild<QListWidget *>("dayViewDetailsList");
                                    if (detailsList) {
                                        detailsList->clear();
                                        // ... (其余显示代码保持不变)

                                        // 创建情显示部件
                                        QWidget* detailWidget = new QWidget;
                                        QVBoxLayout* detailLayout = new QVBoxLayout(detailWidget);
                                        detailLayout->setSpacing(15);
                                        detailLayout->setContentsMargins(15, 15, 15, 15);

                                        // 标题行（颜色圆圈 + 标题）
                                        QWidget* titleWidget = new QWidget;
                                        titleWidget->setStyleSheet("background: transparent; border: none;");
                                        QHBoxLayout* titleLayout = new QHBoxLayout(titleWidget);
                                        titleLayout->setSpacing(10);
                                        titleLayout->setContentsMargins(0, 0, 0, 0);

                                        // 颜色圆圈
                                        QLabel* colorDot = new QLabel;
                                        colorDot->setFixedSize(12, 12);
                                        colorDot->setStyleSheet(QString("background-color: %1; border-radius: 6px; border: none;")
                                                                .arg(event.color.name()));
                                        titleLayout->addWidget(colorDot);

                                        // 标题
                                        QLabel* titleLabel = new QLabel(event.text);
                                        titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: black; background: transparent; border: none;");
                                        titleLayout->addWidget(titleLabel);
                                        titleLayout->addStretch();

                                        detailLayout->addWidget(titleWidget);

                                        // 时间
                                        QLabel* timeLabel = new QLabel(QString("%1 - %2")
                                                                       .arg(event.startTime.toString("HH:mm"))
                                                                       .arg(event.endTime.toString("HH:mm")));
                                        timeLabel->setStyleSheet("color: black; margin-left: 22px; background: transparent; border: none;");
                                        detailLayout->addWidget(timeLabel);

                                        // 详情（如果有）
                                        if (!event.description.isEmpty()) {
                                            QLabel* descLabel = new QLabel(event.description);
                                            descLabel->setWordWrap(true);
                                            descLabel->setMinimumHeight(100);
                                            descLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                                            descLabel->setStyleSheet(R"(
                                                background-color: #f8f9fa;
                                                padding: 15px;
                                                margin-left: 22px;
                                                margin-right: 10px;
                                                color: black;
                                                font-size: 13px;
                                                line-height: 1.5;
                                                border: none;
                                            )");
                                            detailLayout->addWidget(descLabel);
                                        }

                                        detailLayout->addStretch();

                                        // 创建列表项并设置部件
                                        QListWidgetItem* item = new QListWidgetItem(detailsList);
                                        item->setSizeHint(QSize(detailsList->width() - 10, 300));
                                        detailsList->addItem(item);
                                        detailsList->setItemWidget(item, detailWidget);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::setupDayView()
{
    // 确保当前日期有效
    if (!currentDate.isValid()) {
        currentDate = QDate::currentDate();
    }

    // 如果dayView已经存在，先删除它
    if (dayView) {
        dayView->deleteLater();
        dayView = nullptr;
    }

    // 建日视图主容器
    dayView = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(dayView);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 左侧部分
    QWidget* leftWidget = new QWidget;
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setSpacing(10);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    // 顶部年份和日期选择器
    QHBoxLayout* topLayout = new QHBoxLayout;

    // 修改年份标签的显示
    QLabel* yearLabel = new QLabel(QString("%1年").arg(currentDate.year()));
    yearLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #333333;");  // 深灰色

    QPushButton* datePickerBtn = new QPushButton;
    datePickerBtn->setText(currentDate.toString("yyyy-MM-dd"));
    datePickerBtn->setStyleSheet(R"(
        QPushButton {
            background: white;
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 5px 10px;
            color: #666666;  /* 修改字体颜色为中灰色 */
        }
        QPushButton:hover {
            background: #f0f0f0;
        }
    )");

    // 日期择按钮点击事件
    connect(datePickerBtn, &QPushButton::clicked, [this, datePickerBtn]() {
        QCalendarWidget* calendar = new QCalendarWidget(this);
        calendar->setWindowFlags(Qt::Popup);
        calendar->setSelectedDate(currentDate);

        // 设置日历选择器的样式
        calendar->setStyleSheet(R"(
            QCalendarWidget {
                background-color: white;
                color: #666666;  /* 中灰色文字 */
            }
            QCalendarWidget QTableView {
                selection-background-color: #007bff;
                selection-color: white;
            }
            QCalendarWidget QWidget#qt_calendar_navigationbar {
                background-color: white;
            }
            QCalendarWidget QToolButton {
                color: #666666;
                background-color: transparent;
            }
            QCalendarWidget QMenu {
                color: #666666;
            }
        )");

        // 设置位置
        QPoint pos = datePickerBtn->mapToGlobal(QPoint(0, datePickerBtn->height()));
        calendar->move(pos);

        // 处理日期选择
        connect(calendar, &QCalendarWidget::clicked, this, [this, calendar, datePickerBtn](const QDate & date) {
            currentDate = date;
            datePickerBtn->setText(date.toString("yyyy-MM-dd"));
            updateDayView();
            calendar->close();
            calendar->deleteLater();
        });

        calendar->show();
    });

    topLayout->addWidget(yearLabel);
    topLayout->addWidget(datePickerBtn);
    topLayout->addStretch();
    leftLayout->addLayout(topLayout);

    // 日表容器
    QWidget* scheduleContainer = new QWidget;
    scheduleContainer->setStyleSheet(R"(
        QWidget {
            background: #f8f9fa;
            border-radius: 10px;
        }
    )");
    QVBoxLayout* scheduleLayout = new QVBoxLayout(scheduleContainer);
    scheduleLayout->setContentsMargins(15, 15, 15, 15);

    // 日期标题和切换按钮容器
    QWidget* dateTitleContainer = new QWidget;
    QHBoxLayout* dateTitleLayout = new QHBoxLayout(dateTitleContainer);
    dateTitleLayout->setContentsMargins(0, 0, 0, 0);
    dateTitleLayout->setSpacing(10);

    // 上一天按钮
    QPushButton* prevDayBtn = new QPushButton;
    prevDayBtn->setIcon(QIcon(":/img/img/last.png"));
    prevDayBtn->setIconSize(QSize(16, 16));
    prevDayBtn->setFixedSize(28, 28);
    prevDayBtn->setStyleSheet(R"(
        QPushButton {
            background: white;
            border: 1px solid #ddd;
            border-radius: 14px;
            padding: 4px;
        }
        QPushButton:hover {
            background: #f0f0f0;
        }
    )");

    // 日期标题
    QLabel* dateTitle = new QLabel(currentDate.toString("MM月dd日"));
    dateTitle->setObjectName("dayViewDateTitle");
    dateTitle->setAlignment(Qt::AlignCenter);
    dateTitle->setStyleSheet("font-size: 24px; font-weight: bold; margin: 10px 0; color: #333333;");

    // 下一天按钮
    QPushButton* nextDayBtn = new QPushButton;
    nextDayBtn->setIcon(QIcon(":/img/img/next.png"));
    nextDayBtn->setIconSize(QSize(16, 16));
    nextDayBtn->setFixedSize(28, 28);
    nextDayBtn->setStyleSheet(R"(
        QPushButton {
            background: white;
            border: 1px solid #ddd;
            border-radius: 14px;
            padding: 4px;
        }
        QPushButton:hover {
            background: #f0f0f0;
        }
    )");

    // 添加到布局
    dateTitleLayout->addStretch();
    dateTitleLayout->addWidget(prevDayBtn);
    dateTitleLayout->addWidget(dateTitle);
    dateTitleLayout->addWidget(nextDayBtn);
    dateTitleLayout->addStretch();

    // 连接��钮信号
    connect(prevDayBtn, &QPushButton::clicked, [this]() {
        currentDate = currentDate.addDays(-1);
        updateDayView();
    });

    connect(nextDayBtn, &QPushButton::clicked, [this]() {
        currentDate = currentDate.addDays(1);
        updateDayView();
    });

    // 将日期标题容器添加到日程布局中
    scheduleLayout->addWidget(dateTitleContainer);

    // 时间表
    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);  // 始显示水平滚动条
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(R"(
        QScrollArea { 
            border: none; 
        }
        QScrollBar:horizontal {
            height: 12px;
            background: white;
            margin: 0px;
            border: none;
        }
        QScrollBar::handle:horizontal {
            background: #cdcdcd;
            min-width: 20px;
        }
        QScrollBar::add-line:horizontal {
            width: 0px;
            border: none;
            background: none;
        }
        QScrollBar::sub-line:horizontal {
            width: 0px;
            border: none;
            background: none;
        }
        QScrollBar:vertical {
            width: 12px;
            background: white;
            margin: 0px;
            border: none;
        }
        QScrollBar::handle:vertical {
            background: #cdcdcd;
            min-height: 20px;
        }
        QScrollBar::add-line:vertical {
            height: 0px;
            border: none;
            background: none;
        }
        QScrollBar::sub-line:vertical {
            height: 0px;
            border: none;
            background: none;
        }
    )");

    QWidget* timeTableWidget = new QWidget;
    QVBoxLayout* timeTableLayout = new QVBoxLayout(timeTableWidget);
    timeTableLayout->setSpacing(0);
    timeTableLayout->setContentsMargins(0, 0, 0, 0);

    // 添加时间段
    for (int hour = 0; hour < 24; hour += 2) {
        QWidget* timeSlot = new QWidget;
        timeSlot->setStyleSheet(R"(
            QWidget { 
                border-bottom: 1px solid #eee;
                background-color: white;
            }
        )");
        timeSlot->setFixedHeight(80);
        QHBoxLayout* slotLayout = new QHBoxLayout(timeSlot);
        slotLayout->setContentsMargins(0, 0, 0, 0);
        slotLayout->setSpacing(0);

        // 时间标签
        QLabel* timeLabel = new QLabel;
        QString timeText;
        if (hour == 22) {
            // 特殊处理22:00~24:00的情况
            timeText = QString("%1 ~ %2")
                       .arg(QTime(22, 0).toString("HH:mm"))
                       .arg(QTime(0, 0).toString("24:00"));  // 使用24:00而不是00:00
        } else {
            timeText = QString("%1 ~ %2")
                       .arg(QTime(hour, 0).toString("HH:mm"))
                       .arg(QTime(hour + 2, 0).toString("HH:mm"));
        }
        timeLabel->setText(timeText);
        timeLabel->setFixedWidth(120);
        timeLabel->setStyleSheet(R"(
            QLabel { 
                color: #666;
                padding: 5px;
                border-right: 1px solid #eee;
                background: white;
            }
        )");
        timeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

        slotLayout->addWidget(timeLabel);

        // 事件容器（作为表格单元格）
        QWidget* eventCell = new QWidget;
        eventCell->setProperty("timeSlot", hour);
        eventCell->setStyleSheet("background-color: white;");
        QHBoxLayout* cellLayout = new QHBoxLayout(eventCell);
        cellLayout->setContentsMargins(10, 5, 10, 5);
        cellLayout->setSpacing(10);
        cellLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        slotLayout->addWidget(eventCell, 1);  // 事件单元格占据剩余空间
        timeTableLayout->addWidget(timeSlot);
    }

    scrollArea->setWidget(timeTableWidget);
    scheduleLayout->addWidget(scrollArea);
    leftLayout->addWidget(scheduleContainer);
    mainLayout->addWidget(leftWidget, 2);  // 左侧占2份

    // 右侧部分
    QWidget* rightWidget = new QWidget;
    rightWidget->setStyleSheet(R"(
        QWidget {
            background: white;
            border-radius: 10px;
            border: 1px solid #eee;
        }
    )");
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(15, 15, 15, 15);

    QLabel* detailsTitle = new QLabel("日程详情");
    detailsTitle->setStyleSheet("font-size: 16px; font-weight: bold; margin-bottom: 10px; border: none;");
    rightLayout->addWidget(detailsTitle);

    QListWidget* detailsList = new QListWidget;
    detailsList->setObjectName("dayViewDetailsList");  // 加对象名以便后续访问
    detailsList->setStyleSheet(R"(
        QListWidget {
            border: none;
        }
        QListWidget::item {
            padding: 10px;
            border-bottom: 1px solid #eee;
        }
        QListWidget::item:selected {
            background: #f0f0f0;
            color: black;
        }
    )");
    rightLayout->addWidget(detailsList);

    mainLayout->addWidget(rightWidget, 1);  // 右侧��1份

    // 设置日视图位置
    dayView->setParent(centralWidget());
    dayView->setGeometry(centralWidget()->rect());

    // 隐藏其他图
    ui->calendarWidget->hide();
    ui->rightWidget->hide();
    if (weekView) weekView->hide();

    // 显示日视图
    dayView->show();

    // 新日视图内容
    QTimer::singleShot(0, this, &MainWindow::updateDayView);
}

void MainWindow::updateDayView()
{
    if (!dayView || !dayView->isVisible()) return;

    try {
        // 更新日期标题
        QLabel* dateTitle = dayView->findChild<QLabel *>("dayViewDateTitle");
        if (dateTitle) {
            dateTitle->setText(currentDate.toString("MM月dd日"));
        }

        // 获取所有时间段容器
        QScrollArea* scrollArea = dayView->findChild<QScrollArea *>();
        if (!scrollArea || !scrollArea->widget()) return;

        // 获取所有带有 timeSlot 属性的部件
        QList<QWidget *> timeSlots;
        foreach (QWidget* widget, scrollArea->widget()->findChildren<QWidget*>()) {
            if (widget->property("timeSlot").isValid()) {
                timeSlots.append(widget);
            }
        }

        // 清理现有事件显示
        for (QWidget * slot : timeSlots) {
            // 修改这里：先将parent()转换为QWidget
            QWidget* parentWidget = qobject_cast<QWidget *>(slot->parent());
            if (!parentWidget) continue;

            QHBoxLayout* slotLayout = qobject_cast<QHBoxLayout *>(parentWidget->layout());
            if (!slotLayout || slotLayout->count() < 2) continue;

            QWidget* eventCell = slotLayout->itemAt(1)->widget();
            if (!eventCell) continue;

            QHBoxLayout* cellLayout = qobject_cast<QHBoxLayout *>(eventCell->layout());
            if (!cellLayout) continue;

            // 清理现有事件
            while (cellLayout->count() > 0) {
                QLayoutItem* item = cellLayout->takeAt(0);
                if (item->widget()) {
                    item->widget()->deleteLater();
                }
                delete item;
            }
        }

        // 添加事件到对应时间段
        if (eventMap.contains(currentDate)) {
            const QList<EventItem> &events = eventMap[currentDate];
            for (const EventItem& event : events) {
                int startHour = event.startTime.time().hour();

                // 找到对应的时间段
                for (QWidget * slot : timeSlots) {
                    int slotHour = slot->property("timeSlot").toInt();
                    if (slotHour <= startHour && slotHour + 2 > startHour) {
                        QHBoxLayout* slotLayout = qobject_cast<QHBoxLayout *>(slot->layout());
                        if (!slotLayout) continue;

                        // 创建事件显示部件
                        QWidget* eventWidget = new QWidget;
                        eventWidget->setFixedSize(150, 40);
                        eventWidget->setStyleSheet(QString(R"(
                            QWidget {
                                background: %1;
                                border-radius: 4px;
                                margin: 2px;
                                border: 1px solid %2;
                            }
                            QLabel {
                                border: none;
                            }
                        )").arg(event.color.name())
                                                   .arg(event.color.darker(120).name()));

                        QVBoxLayout* eventItemLayout = new QVBoxLayout(eventWidget);  // 修改变量名
                        eventItemLayout->setContentsMargins(4, 2, 4, 2);
                        eventItemLayout->setSpacing(0);

                        // 添加时间标签
                        QLabel* timeLabel = new QLabel(QString("%1-%2")
                                                       .arg(event.startTime.toString("HH:mm"))
                                                       .arg(event.endTime.toString("HH:mm")));
                        timeLabel->setStyleSheet("background: transparent; color: white; font-size: 10px;");
                        timeLabel->setAlignment(Qt::AlignLeft);
                        eventItemLayout->addWidget(timeLabel);  // 使用正确的变量名

                        // 添加标题标签
                        QLabel* titleLabel = new QLabel(event.text);
                        titleLabel->setStyleSheet("background: transparent; color: white; font-weight: bold; font-size: 11px;");
                        titleLabel->setAlignment(Qt::AlignLeft);
                        eventItemLayout->addWidget(titleLabel);  // 使用正确的变量名

                        // 将事件添加到布局中
                        slotLayout->addWidget(eventWidget);

                        // 更新容器宽度
                        int totalWidth = (slotLayout->count() * (150 + 10)) + 20;  // 事件度 + 间距
                        slot->setMinimumWidth(qMax(500, totalWidth));  // 保持最小宽度

                        eventWidget->installEventFilter(this);  // 安装事件过滤器

                        break;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        qDebug() << "Error in updateDayView:" << e.what();
    }
}

void MainWindow::onEventItemDoubleClicked(QListWidgetItem* item)
{
    if (!item) return;

    QDate selectedDate = ui->calendarWidget->selectedDate();
    int currentRow = ui->eventList->row(item);

    if (eventMap.contains(selectedDate) && currentRow < eventMap[selectedDate].size()) {
        const EventItem& event = eventMap[selectedDate][currentRow];

        // 创建详情对话框
        QDialog dialog(this);
        dialog.setWindowTitle(tr("事件详情"));
        dialog.setMinimumWidth(300);
        dialog.setStyleSheet("QDialog { background-color: white; }");  // 设置话框背景为白色

        QVBoxLayout* layout = new QVBoxLayout(&dialog);

        // 标题行（颜色圆圈 + 标题）
        QWidget* titleWidget = new QWidget;
        titleWidget->setStyleSheet("background: transparent;");  // 设置背景透明
        QHBoxLayout* titleLayout = new QHBoxLayout(titleWidget);

        // 颜色圆圈
        QLabel* colorDot = new QLabel;
        colorDot->setFixedSize(12, 12);
        colorDot->setStyleSheet(QString("background-color: %1; border-radius: 6px;")
                                .arg(event.color.name()));
        titleLayout->addWidget(colorDot);

        // 标题
        QLabel* titleLabel = new QLabel(event.text);
        titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; background: transparent;");
        titleLayout->addWidget(titleLabel);
        titleLayout->addStretch();

        layout->addWidget(titleWidget);

        // 时间
        QLabel* timeLabel = new QLabel(QString("%1 - %2")
                                       .arg(event.startTime.toString("HH:mm"))
                                       .arg(event.endTime.toString("HH:mm")));
        timeLabel->setStyleSheet("color: #666666; margin-left: 22px; background: transparent;");
        layout->addWidget(timeLabel);

        // 详情（如果有）
        if (!event.description.isEmpty()) {
            QLabel* descLabel = new QLabel(event.description);
            descLabel->setWordWrap(true);
            descLabel->setStyleSheet(R"(
                background-color: #f8f9fa;
                border-radius: 4px;
                padding: 15px;
                margin: 10px 0;
                margin-left: 22px;
                color: black;
            )");
            layout->addWidget(descLabel);
        }

        // 确定按钮
        QPushButton* okButton = new QPushButton(tr("确定"), &dialog);
        okButton->setStyleSheet(R"(
            QPushButton {
                background-color: #007bff;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 8px 20px;
                min-width: 80px;
            }
            QPushButton:hover {
                background-color: #0056b3;
            }
        )");
        connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);

        layout->addWidget(okButton, 0, Qt::AlignCenter);
        layout->addStretch();

        dialog.exec();
    }
}

void MainWindow::loadEventsFromDatabase()
{
    // 清空现有事件
    eventMap.clear();
    
    // 获取当前显示月份的所有事件
    QDate firstDay(ui->calendarWidget->yearShown(), ui->calendarWidget->monthShown(), 1);
    QDate lastDay = firstDay.addMonths(1).addDays(-1);
    
    auto events = database->getEventsByDateRange(firstDay, lastDay);
    
    // 将事件添加到 eventMap
    for (const auto& pair : events) {
        int id = pair.first;
        const QVariantMap& eventData = pair.second;
        
        EventItem event;
        event.id = id;  // 保存事件ID
        event.text = eventData["title"].toString();
        event.startTime = eventData["start_time"].toDateTime();
        event.endTime = eventData["end_time"].toDateTime();
        event.description = eventData["description"].toString();
        event.color = eventData["color"].value<QColor>();
        
        eventMap[event.startTime.date()].append(event);
    }
    
    // 更新显示
    updateEventList();
}

int MainWindow::saveEventToDatabase(const EventItem& event)
{
    int newId = database->addEvent(
        event.text,
        event.startTime,
        event.endTime,
        event.description,
        event.color
    );
    
    if (newId <= 0) {
        QMessageBox::warning(this, tr("错误"), tr("保存事件失败"));
    }
    return newId;
}

void MainWindow::updateEventInDatabase(int eventId, const EventItem& event)
{
    bool success = database->updateEvent(
        eventId,
        event.text,
        event.startTime,
        event.endTime,
        event.description,
        event.color
    );
    
    if (!success) {
        QMessageBox::warning(this, tr("错误"), tr("更新事件失败"));
    }
}

void MainWindow::deleteEventFromDatabase(int eventId)
{
    bool success = database->deleteEvent(eventId);
    
    if (!success) {
        QMessageBox::warning(this, tr("错误"), tr("删除事件失败"));
    }
}

