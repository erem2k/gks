#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
private slots:
    void updateCount(int);
    void updateReadyStatus();
    void calculate();
    void displayResults(int**, QVector<QString>);

};

#endif // MAINWINDOW_H
