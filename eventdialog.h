#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>
#include <QDateTime>
#include <QColor>

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
                     const QDateTime &endTime, const QString &description = QString(),
                     const QColor &color = Qt::blue);
    QString getEventTitle() const;
    QDateTime getStartTime() const;
    QDateTime getEndTime() const;
    QString getDescription() const;
    QColor getEventColor() const;

private slots:
    void on_startDateTimeEdit_dateTimeChanged(const QDateTime &dateTime);
    void on_colorButton_clicked();
    void updateColorButton();

private:
    Ui::EventDialog *ui;
    QColor eventColor;
};

#endif // EVENTDIALOG_H 