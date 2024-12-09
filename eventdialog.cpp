#include "eventdialog.h"
#include "ui_eventdialog.h"
#include <QMessageBox>
#include <QColorDialog>

EventDialog::EventDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventDialog),
    eventColor(QColor(135, 206, 250))  // 修改默认颜色为浅蓝色
{
    ui->setupUi(this);

    // 设置日期时间编辑器的格式
    ui->startDateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    ui->endDateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");

    // 设置默认时间
    QDateTime currentTime = QDateTime::currentDateTime();
    ui->startDateTimeEdit->setDateTime(currentTime);
    ui->endDateTimeEdit->setDateTime(currentTime.addSecs(3600));

    // 设置对话框样式
    setStyleSheet(R"(
        QDialog {
            background-color: white;
        }
        QLabel {
            color: black;
        }
        QLineEdit, QTextEdit, QDateTimeEdit {
            background-color: white;
            color: black;
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 4px;
        }
        QLineEdit:focus, QTextEdit:focus, QDateTimeEdit:focus {
            border-color: #007bff;
        }
        QPushButton {
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 12px;
            min-width: 80px;
        }
        QPushButton:hover {
            background-color: #0056b3;
        }
        QPushButton#colorButton {
            margin: 4px;
            min-height: 32px;  /* 增加按钮高度 */
        }
        QDialogButtonBox QPushButton {
            min-width: 60px;
        }
        /* 日期时间编辑器的下拉按钮样式 */
        QDateTimeEdit::down-button, QDateTimeEdit::up-button {
            border: none;
            background-color: transparent;
        }
        QDateTimeEdit::down-button:hover, QDateTimeEdit::up-button:hover {
            background-color: #f0f0f0;
        }
        /* 文本框的滚动条样式 */
        QTextEdit QScrollBar:vertical {
            width: 12px;
            background: white;
        }
        QTextEdit QScrollBar::handle:vertical {
            background: #cdcdcd;
            border-radius: 6px;
            min-height: 20px;
        }
        QTextEdit QScrollBar::add-line:vertical,
        QTextEdit QScrollBar::sub-line:vertical {
            height: 0px;
        }
    )");

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
    QString style = QString(R"(
        QPushButton {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 4px;
            padding: 6px 12px;
            min-height: 32px;  /* 增加按钮高度 */
            font-size: 12px;   /* 调整字体大小 */
        }
        QPushButton:hover {
            background-color: %4;
        }
    )")
        .arg(eventColor.name())
        .arg(eventColor.value() < 128 ? "white" : "black")
        .arg(eventColor.darker(120).name())
        .arg(eventColor.lighter(110).name());
    
    ui->colorButton->setStyleSheet(style);
} 