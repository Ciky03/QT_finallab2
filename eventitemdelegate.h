#ifndef EVENTITEMDELEGATE_H
#define EVENTITEMDELEGATE_H

#include <QStyledItemDelegate>

class EventItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit EventItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
};

#endif // EVENTITEMDELEGATE_H 