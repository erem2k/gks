#include "intitemdelegate.h"

IntItemDelegate::IntItemDelegate(QObject *parent)
    : QItemDelegate(parent)
{

}

QWidget* IntItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option,
                  const QModelIndex & index) const
{
    QLineEdit *lineEdit = new QLineEdit(parent);
    QIntValidator *validator = new QIntValidator(0, INT32_MAX, lineEdit);
    lineEdit->setValidator(validator);
    return lineEdit;
}

IntItemDelegate::~IntItemDelegate()
{

}
