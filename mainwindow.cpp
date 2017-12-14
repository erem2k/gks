#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->countSlider->setValue(ui->objectTable->rowCount());

    QObject::connect(ui->countSlider, &QSlider::valueChanged, this, &MainWindow::updateCount);
    QObject::connect(ui->calculateButton, &QPushButton::clicked, this, &MainWindow::calculate);
    QObject::connect(ui->objectTable, &QTableWidget::cellChanged, this, &MainWindow::updateReadyStatus);
    this->setFocusPolicy(Qt::StrongFocus);
}

void MainWindow::specifyGroups(QVector<QVector<int>> ocMatrix, QVector<QVector<int>> groups)
{
    QVector<Group> specGroups;

    //Fetch unique operations for each group
    for (int i = 0; i < groups.size(); i++) {
        QVector<int> uniqueOps;

        for (int objIndex : groups[i])
            for (int j = 0; j < ops_.size(); j++)
                if (ocMatrix[objIndex][j] == 1)
                    if (!uniqueOps.contains(j))
                        uniqueOps.push_back(j);

        specGroups.push_back(Group(groups[i], uniqueOps, i));
    }

    std::sort(specGroups.begin(), specGroups.end(),
           [](const Group &g1, const Group &g2)
             {return (g1.opIndex_.size() > g2.opIndex_.size());});

    //Merging subset groups into their supersets
    bool isComplete = false;
    while (!isComplete) {
        isComplete = true; //If no groups merged on this pass, groups are specified
        for (int i = 0; i < specGroups.size(); i++) {
            for (int j = i + 1; j < specGroups.size(); j++) {
                for (int objectIndex : specGroups[j].objectIndex_) {
                    //Form ops for each object
                    QVector<int> objectOps;
                    //TODO: Redo Group struct to accomodate
                    for (int k = 0; k < ops_.size(); k++)
                        if (ocMatrix[objectIndex][k] == 1)
                            objectOps.push_back(k);
                    //If current object is operationally a subset of supergroup
                    if (isSubset(specGroups[i].opIndex_, objectOps)) {
                        isComplete = false;

                        specGroups[i].objectIndex_.push_back(objectIndex);
                        specGroups[j].objectIndex_.removeOne(objectIndex);
                        //TODO: Redo Group struct to accomodate
                        //Rebuild opIndex of current object
                        specGroups[j].opIndex_.clear();
                        QVector<int> uniqueOps;
                        for (int rObjIndex : specGroups[j].objectIndex_)
                            for (int k = 0; k < ops_.size(); k++)
                                if (ocMatrix[rObjIndex][k] == 1)
                                    if (!uniqueOps.contains(k))
                                        uniqueOps.push_back(k);
                        specGroups[j].opIndex_ = uniqueOps;
                        //Check empty
                        if (specGroups[j].objectIndex_.empty()) {
                            specGroups.removeAt(j);
                            j--;
                        }
                    }
                }
            }
        }
        std::sort(specGroups.begin(), specGroups.end(),
               [](const Group &g1, const Group &g2)
                 {return (g1.opIndex_.size() > g2.opIndex_.size());});
    }

    displayLab2Results(specGroups);

}

void MainWindow::findGroups(QVector<QVector<int>> ocMatrix)
{
    //Build operations matrix
    QVector<QVector<int>> opMatrix(ocMatrix.size(), QVector<int>(ocMatrix.size(), -1));

    for (int i = 1; i < ocMatrix.size(); i++) {
        for (int j = 0; j < i; j++) {
            int nrOps = 0;

            //Counts operations present only in single object from current pair, but not in both
            for (int k = 0; k < ocMatrix[i].size(); k++)
                if (ocMatrix[i][k] - ocMatrix[j][k] != 0)
                    nrOps++;

            opMatrix[i][j] = ops_.size() - nrOps;
        }
    }

    //Form groups
    QVector<QVector<int>> passMatrix = opMatrix;    //We'll need operations matrix for output
    QPoint pMax = getMax(passMatrix);
    int vMax = passMatrix[pMax.x()][pMax.y()];
    QVector<QVector<int>> groups;

    while (vMax != -1) {
        QVector<int> currentGroup = passRow(passMatrix, vMax, pMax.x());

        pMax = getMax(passMatrix);
        vMax = passMatrix[pMax.x()][pMax.y()];
        if (!currentGroup.empty())
            groups.push_back(currentGroup);
    }
    //In some cases, not all objects end up grouped - then they are placed in separate groups
    for (int i = 0; i < passMatrix.size(); i++) {
        bool containsFlag = false;

        for (QVector<int> v : groups)
            if (v.contains(i))
                containsFlag = true;

        if(containsFlag == false)
            groups.push_back(QVector<int>(1, i));
    }

    displayLab1Results(opMatrix, groups);
    specifyGroups(ocMatrix, groups);
}

void MainWindow::updateCount(int newSize)
{
    ui->countLCD->display(newSize);
    ui->objectTable->setRowCount(newSize);
    this->updateReadyStatus();
}

void MainWindow::calculate()
{
    ops_.clear();
    //Parse data from cells and build occurence matrix
    ui->statusLabel->setText("Calculation started!");

    //Get all available operations
    for (int i=0; i<ui->objectTable->rowCount(); i++) {
        QStringList slist = ui->objectTable->item(i,0)->text().split(';');

        for(QString& s : slist) {
            if (!ops_.contains(s))
                ops_.push_back(s);
        }
    }

    //Fill occurence matrix
    QVector<QVector<int>> occurenceMatrix (ui->objectTable->rowCount(),QVector<int>(ops_.size(),0));

    for (int i = 0; i < ui->objectTable->rowCount(); i++) {
        for (int j = 0; j < ops_.size(); j++) {

            QStringList sortedList = ui->objectTable->item(i,0)->text().split(';');
            std::sort(sortedList.begin(),sortedList.end());

            if (sortedList.contains(ops_.at(j)))
                occurenceMatrix[i][j]=1;
        }
    }

    findGroups(occurenceMatrix);
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

void MainWindow::displayLab1Results(QVector<QVector<int>> resMatrix, QVector<QVector<int>> groups)
{
    ui->statusLabel->setText("Done!");
    ui->resultTextEdit->clear();
    ui->resultTextEdit->appendPlainText("Resemblance matrix: \n");

    for (int i = 0; i < resMatrix.size(); i++) {
        QString s="";
        for (int j = 0; j < resMatrix.size(); j++) {
            if (resMatrix[i][j] != -1)
                s+=QString::number(resMatrix[i][j]);
            else
                s+="-";
            s+=" ";
        }
        ui->resultTextEdit->appendPlainText(s);
    }

    ui->resultTextEdit->appendPlainText("\nGroups:");
    for (int i = 0; i < groups.size(); i++) {
        QString s= "", subs="";
        s += QString::number(i + 1);
        s += ": ";

        for (int objIndex : groups[i]) {
            subs += QString::number(objIndex + 1);
            subs += " ";
        }

        s += subs;
        ui->resultTextEdit->appendPlainText(s);
    }
}

void MainWindow::displayLab2Results(QVector<Group> specifiedGroups)
{
    ui->resultTextEdit->appendPlainText("\nSpecified groups:");

    for (Group g : specifiedGroups) {
        QString s= "\n", subs = "\nGroup operations: ";
        s += QString::number(g.id_ + 1);
        s += ": ";

        for (int objIndex : g.objectIndex_) {
            s += QString::number(objIndex + 1);
            s += " ";
        }
        for (int opIndex : g.opIndex_) {
            subs += ops_.at(opIndex);
            subs += " ";
        }

        s += subs;

        ui->resultTextEdit->appendPlainText(s);
    }
}

bool MainWindow::isSubset(QVector<int> A, QVector<int> B)
{
    std::sort(A.begin(), A.end());
    std::sort(B.begin(), B.end());
    return std::includes(A.begin(), A.end(), B.begin(), B.end());
}

QPoint MainWindow::getMax(QVector<QVector<int>> ldMatrix)
{
    QPoint result(0,0);
    for (int i = 0; i < ldMatrix.size(); i++)
        for (int j = 0; j < i; j++)
            if (ldMatrix[result.x()][result.y()] < ldMatrix[i][j])
                result = QPoint(i,j);

    return result;
}

QVector<int> MainWindow::passRow(QVector<QVector<int>>& opsMatrix, int maxVal, int targetRow)
{
    QVector<int> passObjects;

    for (int i = 0; i < targetRow; i++) {
        QVector<int> cpassObjects;
        if (opsMatrix[targetRow][i] == maxVal) {
            if (!passObjects.contains(targetRow))
                passObjects.push_back(targetRow);
            passObjects.push_back(i);
            opsMatrix[targetRow][i] = -1;   //To avoid column pass -> row pass loop on same element
            cpassObjects = passColumn(opsMatrix, maxVal, i);

            for (int ind : cpassObjects)
                if (!passObjects.contains(ind))
                    passObjects.push_back(ind);
        }
        else
            opsMatrix[targetRow][i] = -1;
    }
    //Strip the corresponding column
    for (int i = targetRow + 1; i < opsMatrix.size(); i++)
        opsMatrix[i][targetRow] = -1;
    return passObjects;
}

QVector<int> MainWindow::passColumn(QVector<QVector<int>>& opsMatrix, int maxVal, int targetCol)
{
    QVector<int> passObjects;

    for (int i = targetCol + 1; i < opsMatrix.size(); i++) {
        QVector<int> rpassObjects;
        if (opsMatrix[i][targetCol] == maxVal) {
            if (!passObjects.contains(targetCol))
                passObjects.push_back(targetCol);
            passObjects.push_back(i);
            opsMatrix[i][targetCol] = -1;   //To avoid row pass -> column pass loop on same element
            rpassObjects = passRow(opsMatrix, maxVal, i);

            for (int ind : rpassObjects)
                if (!passObjects.contains(ind))
                    passObjects.push_back(ind);
        }
        else
            opsMatrix[i][targetCol] = -1;
    }
    //Strip the corresponding row
    for (int& ind : opsMatrix[targetCol])
        ind = -1;
    return passObjects;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->matches(QKeySequence::Paste)) {
        QClipboard *clipboard = QApplication::clipboard();
        if (clipboard->mimeData()->hasText()) {
            QStringList slist = clipboard->text().split('\n');
            auto selectedItems = ui->objectTable->selectedItems();

            for (int i = 0; i < selectedItems.size(); i++) {
                if (i < slist.size())
                    selectedItems[i]->setText(slist[i]);
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
