#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>
#include <QDateTime>

namespace Ui {
class EventDialog;
}

class EventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EventDialog(QWidget *parent = nullptr);
    ~EventDialog();

    void setEventData(const QString &title, const QDateTime &startTime,
                     const QDateTime &endTime, const QString &description = QString());
    QString getEventTitle() const;
    QDateTime getStartTime() const;
    QDateTime getEndTime() const;
    QString getDescription() const;

private slots:
    void on_startDateTimeEdit_dateTimeChanged(const QDateTime &dateTime);

private:
    Ui::EventDialog *ui;
};

#endif // EVENTDIALOG_H 