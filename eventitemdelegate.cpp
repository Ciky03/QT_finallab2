#include "eventitemdelegate.h"
#include <QPainter>

EventItemDelegate::EventItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void EventItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    if (!index.isValid()) return;

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // 保存画笔状态
    painter->save();

    // 绘制背景
    if (opt.state & QStyle::State_Selected) {
        painter->fillRect(opt.rect, opt.palette.highlight());
    }

    // 计算颜色圆圈的位置和大小
    int diameter = opt.rect.height() - 8;  // 圆圈直径
    QRect circleRect = opt.rect;
    circleRect.setLeft(circleRect.left() + 4);
    circleRect.setWidth(diameter);
    circleRect.setHeight(diameter);
    circleRect.moveCenter(QPoint(circleRect.center().x(), opt.rect.center().y()));

    // 绘制颜色圆圈
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(index.data(Qt::BackgroundRole).value<QColor>());
    painter->drawEllipse(circleRect);

    // 绘制文本
    QRect textRect = opt.rect;
    textRect.setLeft(circleRect.right() + 8);  // 文本左边界移到圆圈右侧
    
    QColor textColor = (opt.state & QStyle::State_Selected) ? 
                      opt.palette.highlightedText().color() : 
                      opt.palette.text().color();
    painter->setPen(textColor);
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, index.data().toString());

    // 恢复画笔状态
    painter->restore();
}

QSize EventItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(size.height() + 4);  // 增加一点高度以容纳圆圈
    return size;
} 