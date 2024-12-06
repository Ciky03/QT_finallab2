#include "eventdialog.h"
#include "ui_eventdialog.h"
#include <QMessageBox>

EventDialog::EventDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventDialog)
{
    ui->setupUi(this);

    // 设置日期时间编辑器的格式
    ui->startDateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    ui->endDateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");

    // 设置默认时间
    QDateTime currentTime = QDateTime::currentDateTime();
    ui->startDateTimeEdit->setDateTime(currentTime);
    ui->endDateTimeEdit->setDateTime(currentTime.addSecs(3600)); // 默认持续1小时
}

EventDialog::~EventDialog()
{
    delete ui;
}

void EventDialog::setEventData(const QString &title, const QDateTime &startTime,
                             const QDateTime &endTime, const QString &description)
{
    ui->titleLineEdit->setText(title);
    ui->startDateTimeEdit->setDateTime(startTime);
    ui->endDateTimeEdit->setDateTime(endTime);
    ui->descriptionTextEdit->setPlainText(description);
}

QString EventDialog::getEventTitle() const
{
    return ui->titleLineEdit->text();
}

QDateTime EventDialog::getStartTime() const
{
    return ui->startDateTimeEdit->dateTime();
}

QDateTime EventDialog::getEndTime() const
{
    return ui->endDateTimeEdit->dateTime();
}

QString EventDialog::getDescription() const
{
    return ui->descriptionTextEdit->toPlainText();
}

void EventDialog::on_startDateTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    // 确保结束时间不早于开始时间
    if (ui->endDateTimeEdit->dateTime() < dateTime) {
        ui->endDateTimeEdit->setDateTime(dateTime.addSecs(3600));
    }
} 