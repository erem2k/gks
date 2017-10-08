#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->countSlider->setValue(ui->objectTable->rowCount());

    QObject::connect(ui->countSlider, &QSlider::valueChanged, this, &MainWindow::updateCount);
    QObject::connect(ui->calculateButton, &QPushButton::clicked, this, &MainWindow::calculate);
    QObject::connect(ui->objectTable, &QTableWidget::cellChanged, this, &MainWindow::updateReadyStatus);
}

void MainWindow::updateCount(int newSize)
{
    ui->countLCD->display(newSize);
    ui->objectTable->setRowCount(newSize);
    this->updateReadyStatus();
}

void MainWindow::calculate()
{
    //Parse data from cells and build occurence matrix
    ui->statusLabel->setText("Calculation started!");

    QVector<QString> ops;

    //Get all available operations
    for (int i=0; i<ui->objectTable->rowCount(); i++) {
        QStringList slist = ui->objectTable->item(i,0)->text().split(';');

        for(QString& s : slist) {
            if (!ops.contains(s))
                ops.push_back(s);
        }
    }

    //Fill occurence matrix
    //TODO: Proper smart pointer or container implementation
    int** occurenceMatrix = new int*[ui->objectTable->rowCount()];
    for (int i=0; i<ui->objectTable->rowCount(); i++) {
        occurenceMatrix[i] = new int[ops.size()];

        for (int j=0; j<ops.size(); j++) {

            QStringList sortedList = ui->objectTable->item(i,0)->text().split(';');
            std::sort(sortedList.begin(),sortedList.end());

            if (sortedList.contains(ops.at(j)))
                occurenceMatrix[i][j]=1;
            else
                occurenceMatrix[i][j]=0;
        }
    }

    //Call second part to continue
}

void MainWindow::updateReadyStatus()
{
    //Check if all cells have data inside, then enable or disable calculation
    for (int i=0; i<ui->objectTable->rowCount(); i++) {
        if (ui->objectTable->item(i,0) == Q_NULLPTR) {
            ui->calculateButton->setEnabled(false);
            ui->statusLabel->setText("To start calculations, input data in all object fields. Use ';' as separator");
            return;
        }
        else {
            if (ui->objectTable->item(i,0)->text() == "") {
                ui->calculateButton->setEnabled(false);
                ui->statusLabel->setText("To start calculations, input data in all object fields. Use ';' as separator");
                return;
            }
        }
    }
    ui->calculateButton->setEnabled(true);
    ui->statusLabel->setText("Ready!");
}

void MainWindow::displayResults(int** resMatrix, QVector<QString> groups)
{
    ui->statusLabel->setText("Done!");
    ui->resultTextEdit->clear();
    ui->resultTextEdit->appendPlainText("Resemblace matrix: \n");

    for (int i=0; i<ui->objectTable->rowCount(); i++) {
        for (int j=0; j<ui->objectTable->rowCount(); j++)
            ui->resultTextEdit->appendPlainText(QString::number(resMatrix[i][j]).append(" "));
        ui->resultTextEdit->appendPlainText("\n");
    }

    ui->resultTextEdit->appendPlainText("\n\nGroups: \n");
    for (int i=0; i<groups.size(); i++) {
        ui->resultTextEdit->appendPlainText(QString::number(i+1).append(": "));
        ui->resultTextEdit->appendPlainText(groups.at(i));
        ui->resultTextEdit->appendPlainText("\n");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
