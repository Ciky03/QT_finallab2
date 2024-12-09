#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMap>
#include <QDate>
#include <QList>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QButtonGroup>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QCursor>
#include <QTimer>
#include <QDebug>
#include "idatabase.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_new_triggered();
    void on_action_exit_triggered();
    void on_action_day_view_triggered();
    void on_action_week_view_triggered();
    void on_action_month_view_triggered();
    void on_action_about_triggered();
    void on_calendarWidget_selectionChanged();
    void updateEventList();
    void on_action_edit_triggered();
    void on_action_delete_triggered();
    void handleEventDrop(const QDate& newDate, int eventIndex);
    void onEventItemDoubleClicked(QListWidgetItem* item);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    struct EventItem {
        int id;
        QString text;
        QColor color;
        QDateTime startTime;
        QDateTime endTime;
        QString description;

        bool operator==(const EventItem& other) const {
            return id == other.id &&
                   text == other.text &&
                   color == other.color &&
                   startTime == other.startTime &&
                   endTime == other.endTime &&
                   description == other.description;
        }
        
        EventItem() : id(-1) {}
    };

    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    QMap<QDate, QList<EventItem>> eventMap;  // 用于存储不同日期的事件
    QWidget *weekView = nullptr;
    QDate currentWeekStart;
    QListWidget *eventListWidget;
    QWidget *dayView = nullptr;  // 添加日视图指针
    QDate currentDate;           // 添加当前日期

    IDatabase* database;
    
    void loadEventsFromDatabase();
    void saveEventToDatabase(const EventItem& event);
    void updateEventInDatabase(int eventId, const EventItem& event);
    void deleteEventFromDatabase(int eventId);

    void setupWeekView();
    void updateWeekView();
    void updateWeekEvents(const QDate& date);
    void showPreviousWeek();
    void showNextWeek();
    void setupDayView();         // 设置日视图
    void updateDayView();        // 更新日视图
    void updateDayEvents();      // 更新日视图事件
};
#endif // MAINWINDOW_H
