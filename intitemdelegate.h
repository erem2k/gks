#ifndef INTITEMDELEGATE_H
#define INTITEMDELEGATE_H

#include <QItemDelegate>
#include <QLineEdit>
#include <QIntValidator>

class IntItemDelegate : public QItemDelegate
{
public:
    IntItemDelegate(QObject* parent = Q_NULLPTR);
    ~IntItemDelegate();

    QWidget* createEditor(QWidget*, const QStyleOptionViewItem&,
                      const QModelIndex&) const override;
};

#endif // INTITEMDELEGATE_H
