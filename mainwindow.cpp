#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->setItemDelegate(new IntItemDelegate);

    topGroupOffset_=QPoint(30,0);
    lowGroupOffset_=QPoint(30,20);

    textEditOffset_=QSize(60,0);
    windowSizeOffset_=QSize(60,0);

    ui->tableWidget->horizontalHeader()->setVisible(true);

    //Triangulate initial table
    for (int i = 0; i<ui->tableWidget->rowCount(); i++) {
        for (int j = i; j<ui->tableWidget->columnCount(); j++) {
            QTableWidgetItem *item = new QTableWidgetItem();

            item->setFlags(item->flags()^Qt::ItemIsEnabled);
            if (j == i)
                item->setBackgroundColor(QColor::fromRgb(247,247,247));
            else
                item->setBackgroundColor(QColor::fromRgb(240,240,240));

            ui->tableWidget->setItem(i, j, item);
        }
    }

    QObject::connect(ui->increaseSizeButton, SIGNAL(clicked(bool)), this, SLOT(increaseTableSize()));
    QObject::connect(ui->decreaseSizeButton, SIGNAL(clicked(bool)), this, SLOT(decreaseTableSize()));
    QObject::connect(ui->startCalcButton, &QPushButton::clicked, this, &MainWindow::calculate);
}

void MainWindow::increaseTableSize()
{
    ui->tableWidget->insertColumn(ui->tableWidget->columnCount()-1);
    ui->tableWidget->insertRow(ui->tableWidget->rowCount()-1);

    ui->sizeLabel->setText(QString().number(ui->tableWidget->rowCount()));

    adjustUi(true);
}

void MainWindow::decreaseTableSize()
{
    ui->tableWidget->removeColumn(ui->tableWidget->columnCount()-1);
    ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);

    ui->sizeLabel->setText(QString().number(ui->tableWidget->rowCount()));

    adjustUi(false);
}

void MainWindow::calculate()
{

}

void MainWindow::adjustUi(bool isExpanding)
{
    //Enable\disable buttons
    if (ui->tableWidget->columnCount() == 20)
        ui->increaseSizeButton->setDisabled(true);
    if (ui->tableWidget->columnCount() <= 19)
        ui->increaseSizeButton->setEnabled(true);

    if (ui->tableWidget->columnCount() == 2)
        ui->decreaseSizeButton->setDisabled(true);
    if (ui->tableWidget->columnCount() >= 3)
        ui->decreaseSizeButton->setEnabled(true);

    //"Triangulate" table, sans diagonal
    for (int i = 0; i<ui->tableWidget->rowCount(); i++) {
        for (int j = i; j<ui->tableWidget->columnCount(); j++) {
            QTableWidgetItem *item = new QTableWidgetItem();

            item->setFlags(item->flags()^Qt::ItemIsEnabled);
            if (i == j)
                item->setBackgroundColor(QColor::fromRgb(247, 247, 247));
            else
                item->setBackgroundColor(QColor::fromRgb(240, 240, 240));

            ui->tableWidget->setItem(i, j, item);
        }
    }

    //Adjust widget positions
    if (isExpanding && ui->tableWidget->columnCount()>4) {
        ui->tableWidget->adjustSize();

        //QTableWidget::adjustSize() causes overlap on 11x11 table, need to set size manually
        if(ui->tableWidget->columnCount()==11)
            ui->tableWidget->resize(684,245);

        if(ui->tableWidget->columnCount()>5)
            windowSizeOffset_=QSize(60,20);

        this->setFixedSize(this->size()+windowSizeOffset_);

        ui->sizeInfoLabel->move(ui->sizeInfoLabel->pos()+topGroupOffset_);
        ui->decreaseSizeButton->move(ui->decreaseSizeButton->pos()+topGroupOffset_);
        ui->sizeLabel->move(ui->sizeLabel->pos()+topGroupOffset_);
        ui->increaseSizeButton->move(ui->increaseSizeButton->pos()+topGroupOffset_);

        ui->startCalcButton->move(ui->startCalcButton->pos()+lowGroupOffset_);

        ui->resultTextEdit->setFixedSize(ui->resultTextEdit->size()+textEditOffset_);
    }
    if (!isExpanding && ui->tableWidget->columnCount() >= 4) {
        ui->tableWidget->adjustSize();

        if(ui->tableWidget->columnCount()<4)
            windowSizeOffset_=QSize(60,0);

        this->setFixedSize(this->size()-windowSizeOffset_);

        ui->sizeInfoLabel->move(ui->sizeInfoLabel->pos()-topGroupOffset_);
        ui->decreaseSizeButton->move(ui->decreaseSizeButton->pos()-topGroupOffset_);
        ui->sizeLabel->move(ui->sizeLabel->pos()-topGroupOffset_);
        ui->increaseSizeButton->move(ui->increaseSizeButton->pos()-topGroupOffset_);

        ui->startCalcButton->move(ui->startCalcButton->pos()-lowGroupOffset_);

        ui->resultTextEdit->setFixedSize(ui->resultTextEdit->size()-textEditOffset_);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
