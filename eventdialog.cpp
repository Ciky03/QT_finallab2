#include "eventdialog.h"
#include "ui_eventdialog.h"
#include <QMessageBox>
#include <QColorDialog>

EventDialog::EventDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventDialog),
    eventColor(Qt::blue)  // 默认蓝色
{
    ui->setupUi(this);

    // 设置日期时间编辑器的格式
    ui->startDateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    ui->endDateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");

    // 设置默认时间
    QDateTime currentTime = QDateTime::currentDateTime();
    ui->startDateTimeEdit->setDateTime(currentTime);
    ui->endDateTimeEdit->setDateTime(currentTime.addSecs(3600));

    // 更新颜色按钮的显示
    updateColorButton();
}

EventDialog::~EventDialog()
{
    delete ui;
}

void EventDialog::setEventData(const QString &title, const QDateTime &startTime,
                             const QDateTime &endTime, const QString &description,
                             const QColor &color)
{
    ui->titleLineEdit->setText(title);
    ui->startDateTimeEdit->setDateTime(startTime);
    ui->endDateTimeEdit->setDateTime(endTime);
    ui->descriptionTextEdit->setPlainText(description);
    eventColor = color;
    updateColorButton();
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

QColor EventDialog::getEventColor() const
{
    return eventColor;
}

void EventDialog::on_startDateTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    // 确保结束时间不早于开始时间
    if (ui->endDateTimeEdit->dateTime() < dateTime) {
        ui->endDateTimeEdit->setDateTime(dateTime.addSecs(3600));
    }
}

void EventDialog::on_colorButton_clicked()
{
    QColor newColor = QColorDialog::getColor(eventColor, this, tr("选择事件颜色"));
    if (newColor.isValid()) {
        eventColor = newColor;
        updateColorButton();
    }
}

void EventDialog::updateColorButton()
{
    // 设置按钮的背景色和文字颜色
    QString style = QString("QPushButton { background-color: %1; color: %2; }")
                       .arg(eventColor.name())
                       .arg(eventColor.value() < 128 ? "white" : "black");
    ui->colorButton->setStyleSheet(style);
} 