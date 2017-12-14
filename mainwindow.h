#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QClipboard>
#include <QMimeData>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    struct Group
    {
        int id_;
        QVector<int> objectIndex_;
        QVector<int> opIndex_;
        Group() {}
        Group(QVector<int> objects, QVector<int> operations, int id) :
            id_(id), objectIndex_(objects), opIndex_(operations)  {}
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void keyPressEvent(QKeyEvent *) override;
private:
    Ui::MainWindow *ui;
    void findGroups(QVector<QVector<int>>);
    void specifyGroups(QVector<QVector<int>>, QVector<QVector<int>>);
    bool inline isSubset(QVector<int>, QVector<int>);

    //Lab 1-specific
    QVector<int> passRow(QVector<QVector<int>>&, int, int);
    QVector<int> passColumn(QVector<QVector<int>>&, int, int);
    QPoint getMax(QVector<QVector<int>>);

    QVector<QString> ops_;
private slots:
    void updateCount(int);
    void updateReadyStatus();
    void calculate();
    void displayLab1Results(QVector<QVector<int>>, QVector<QVector<int>>);
    void displayLab2Results(QVector<Group>);
};

#endif // MAINWINDOW_H
