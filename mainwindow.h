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

private:
    struct EventItem {
        QString text;
        QColor color;
        QDateTime startTime;
        QDateTime endTime;
        QString description;
    };

    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    QMap<QDate, QList<EventItem>> eventMap;  // 用于存储不同日期的事件
    QWidget *weekView = nullptr;
    QDate currentWeekStart;
    QListWidget *eventListWidget;

    void setupWeekView();
    void updateWeekView();
    void updateWeekEvents(const QDate& date);
    void showPreviousWeek();
    void showNextWeek();
};
#endif // MAINWINDOW_H
