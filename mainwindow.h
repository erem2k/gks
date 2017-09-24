#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "intitemdelegate.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void increaseTableSize();
    void decreaseTableSize();
    void calculate();
private:
    Ui::MainWindow *ui;

    QPoint topGroupOffset_;      //Size offset for widgets, located over the table
    QPoint lowGroupOffset_;      //Size offset for widgets, located under the table

    QSize textEditOffset_;
    QSize windowSizeOffset_;

    void adjustUi(bool);
};

#endif // MAINWINDOW_H
