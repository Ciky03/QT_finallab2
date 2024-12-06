#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

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

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
};
#endif // MAINWINDOW_H
