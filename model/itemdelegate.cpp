#include "itemdelegate.h"
#include <QPainter>
#include <QMouseEvent>

ItemDelegate::ItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{}

void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    QRect buttonRect = QRect(option.rect.right() - 30, option.rect.top() + 5, 20, 20);
    painter->drawRect(buttonRect);
    painter->drawText(buttonRect, Qt::AlignCenter, "X");
}

bool ItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                               const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        QRect buttonRect = QRect(option.rect.right() - 30, option.rect.top() + 5, 20, 20);
        if (buttonRect.contains(mouseEvent->pos())) {
            emit deleteRequested(index.row());
            return true;
        }
    }
    return false;
}
