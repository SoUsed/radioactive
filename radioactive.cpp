#include "radioactive.h"
#include "ui_radioactive.h"

#include "connectisotopedb.h"

#include <iostream>
#include <QWidget>
#include <QFile>
#include <QAction>
#include <QMenuBar>
#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QtCharts>
#include <QtSql>
#include <QTime>
#include <Qt>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QLayout>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMenu>
#include <QThread>

/*!
    \brief Класс, в котором реализован интерфейс и контроллер

    Конструктор генерирует окно с виджетом для графика и двумя лэйаутами для ввода данных.
    Обьект класса посылает всякие запросы в isotopedb, управляет классом radioactivemix и должен логировать происходящее
*/
radioactive::radioactive(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::radioactive)
{
    // log.txt file creating
    log_to_save.setFileName("log.txt");
    log_to_save.open(QIODevice::ReadWrite | QIODevice::Text);
    log_to_save.remove();
    log_to_save.open(QIODevice::ReadWrite | QIODevice::Text);
    //

    unlocked = false; // is password unlocked = false



    ui->setupUi(this);
    newLog("App Started");

    createMenu();
    newLog("Menu initialized");

    createEditGroup();
    createProcessGroup();
    newLog("Edit and Process boxes Created");

    createPlot();
    newLog("Plot widget created");

    initializeModel();
    newLog("SQL DB initialized");


    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->setMenuBar(menuBar);

    mainLayout->addWidget(plotWidget,4);

    mainLayout->addWidget(editGroupBox,1);
    mainLayout->addWidget(processGroupBox,1);

    setLayout(mainLayout);

    theMix = new radioactivemix;
}

radioactive::~radioactive()
{
    //getMainLog();
    delete ui;
}

/*!
 Выводит лог в консоль дебага, так же делает запись в основной лог (и в переменную, и в файл)
 */
void radioactive::newLog(QString nLog)
{
    QString logToPush = "[" + QTime::currentTime().toString("HH:mm:ss") + "] " +  nLog;
    mainLog.push_back(logToPush);
    qDebug() << QString(logToPush);
    QTextStream log_qts(&log_to_save);
    log_qts<<logToPush << "\n";

}

/*!
    \brief Метод, создающий верхнюю менюшку окна

    Создает меню управления окном, логами и базой данных.
    Создает соединения между сигналами меню и слотами класса

 */
void radioactive::createMenu()
{
        menuBar = new QMenuBar;
        fileMenu = new QMenu(tr("&File"), this);
        exitAction = fileMenu->addAction(tr("E&xit"));
        minimaxiAction = fileMenu->addAction(tr("Maximize/minimize"));
        minimaxiAction->setShortcut(QKeySequence("F1"));
        menuBar->addMenu(fileMenu);
        connect(exitAction, SIGNAL(triggered()) , this , SLOT(close()) );
        connect(minimaxiAction, SIGNAL(triggered()) ,this, SLOT(minimize_maximize()) );

        logMenu = new QMenu(tr("&Log"),this);
        getLogAction = logMenu->addAction(tr("Show log"));
        getLogAction->setShortcut(QKeySequence("F2"));
        menuBar->addMenu(logMenu);
        connect(getLogAction, SIGNAL(triggered()), this , SLOT(getMainLog()) );

        dbMenu = new QMenu(tr("&Database"));
        showIsotopesDataBase = dbMenu->addAction(tr("Show isotopes database"));
        showIsotopesDataBase->setShortcut(QKeySequence("F3"));

        openDBConsole = dbMenu->addAction(tr("SQL request to isotopes database"));
        openDBConsole->setShortcut(QKeySequence("F4"));
        connect(openDBConsole, SIGNAL(triggered()), this, SLOT(openConsoleDB()) );

        menuBar->addMenu(dbMenu);
        connect(showIsotopesDataBase, SIGNAL(triggered()), this, SLOT(showTable()) );
}

/*!
    \brief Метод, создающий групбокс для управления смесью

    Создает формы для ввода и просмотра данных о радиоактивной смеси

 */
void radioactive::createEditGroup()
{
    editGroupBox = new QGroupBox(tr("Edit"));
    QGridLayout *eGB = new QGridLayout;

    Edit1CollumnDesc = new QLabel("Number of molecules to add:");
    eGB->addWidget(Edit1CollumnDesc,1,1);

    numOfMoleculesToAdd = new QTextEdit;
    numOfMoleculesToAdd->setPlaceholderText("number");
    eGB->addWidget(numOfMoleculesToAdd,2,1);

    isotopeToAdd = new QComboBox;
    getIsotopeList();
    isotopeToAdd->setPlaceholderText("Choose Isotope");
    for(int i=0;i<isotopesList.size();i++)
    {
        isotopeToAdd->addItem(isotopesList[i].second);
    }

    eGB->addWidget(isotopeToAdd,3,1);

    unitsToAdd = new QComboBox;

    unitsToAdd->setPlaceholderText("Choose units");

    unitsToAdd->addItem("Atoms");
    //unitsToAdd->addItem("Kg");

    eGB->addWidget(unitsToAdd,4,1);

    addButton = new QPushButton("Add");
    connect(addButton,SIGNAL(clicked()),this,SLOT(addButtonClicked()));
    eGB->addWidget(addButton,5,1);

    rmTable = new QTableWidget(0,2);

    QTableWidgetItem *header0;
    QTableWidgetItem *header1;

    header0 = new QTableWidgetItem("Name");
    header1 = new QTableWidgetItem("Quantity");

    rmTable->setHorizontalHeaderItem(0,header0);
    rmTable->setHorizontalHeaderItem(1,header1);

    rmTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    eGB->addWidget(rmTable,1,2,5,1);

    QLabel *lbl;
    lbl = new QLabel("Mix controls");

    eGB->addWidget(lbl,1,3);

    deleteChosenRM = new QPushButton("Delete chosen iso");
    connect(deleteChosenRM, SIGNAL(clicked()), this, SLOT(deleteChosenRM_fun()) );

    deleteAllRM = new QPushButton("Reset mix");
    connect(deleteAllRM, SIGNAL(clicked()), this, SLOT(deleteAllRM_fun()) );

    eGB->addWidget(deleteChosenRM,2,3);
    eGB->addWidget(deleteAllRM,3,3);

    statusLabel = new QLabel("Status: Ready");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("QLabel { background-color : green; color : black; }");

    eGB->addWidget(statusLabel,5,3);

    eGB->setColumnStretch(1,4);
    eGB->setColumnStretch(2,12);
    eGB->setColumnStretch(3,2);
    eGB->setRowStretch(1,3);
    eGB->setRowStretch(2,4);
    eGB->setRowStretch(3,4);
    eGB->setRowStretch(4,4);
    eGB->setRowStretch(5,5);

    editGroupBox->setLayout(eGB);

}

/*!
    \brief Метод, создающий окно настройки итераций

    Создает меню управления временем итерации, количеством итераций

 */

void radioactive::createProcessGroup()
{
    processGroupBox = new QGroupBox(tr("Process"));
    QGridLayout *pGB = new QGridLayout;

    Process1CollumnDesc = new QLabel("Iteration interval:");
    pGB->addWidget(Process1CollumnDesc,1,1);

    timeOfIteration = new QTextEdit;
    timeOfIteration->setPlaceholderText("number");
    pGB->addWidget(timeOfIteration,2,1);

    unitsIteration = new QComboBox;

    unitsIteration->setPlaceholderText("Choose units");

    unitsIteration->addItem("seconds");
    //unitsIteration->addItem("minutes");
    //unitsIteration->addItem("hours");
    //unitsIteration->addItem("days");
    //unitsIteration->addItem("years");

    pGB->addWidget(unitsIteration,3,1);


    setTimeButton = new QPushButton("Set");
    pGB->addWidget(setTimeButton,4,1);

    /*

    Process2CollumnDesc = new QLabel("Update interval:");
    pGB->addWidget(Process2CollumnDesc,1,2);


    updateTime = new QTextEdit;
    updateTime->setPlaceholderText("number");
    pGB->addWidget(updateTime,2,2);

    unitsUpdate = new QComboBox;
    unitsUpdate->setCurrentText("2ergty3g");
    pGB->addWidget(unitsUpdate,3,2);

    setUpdateButton = new QPushButton("Set");
    pGB->addWidget(setUpdateButton,4,2);

    Process4CollumnDesc = new QLabel("Set graph's time units(DON'T use after long iterations):");
    pGB->addWidget(Process4CollumnDesc,1,4);*/

    /*graphUnitsBox = new QComboBox;
    pGB->addWidget(graphUnitsBox,2,4);

    setGraphUnits = new QPushButton("Set");
    pGB->addWidget(setGraphUnits,3,4);

    QHBoxLayout *prL = new QHBoxLayout;

    pauseunpause = new QLabel("Process:");
    prL->addWidget(pauseunpause,4);*/

    /*continueIt = new QPushButton("Continue/start");
    prL->addWidget(continueIt,5);

    stopIt = new QPushButton("Stop");
    prL->addWidget(stopIt,3);

    pGB->addLayout(prL,4,4);*/

    QLabel *lbl;
    lbl = new QLabel("Number of iterations:");
    pGB->addWidget(lbl,1,2);

    numOfIter = new QTextEdit;
    numOfIter->setPlaceholderText("Number of iterations");

    pGB->addWidget(numOfIter,2,2);

    unitsOfIter = new QComboBox;

    unitsOfIter->setPlaceholderText("Choose units");

    unitsOfIter->addItem("times");
    //unitsOfIter->addItem("seconds");
    //unitsOfIter->addItem("minutes");
    //unitsOfIter->addItem("hours");
    //unitsOfIter->addItem("days");
    //unitsOfIter->addItem("years");

    pGB->addWidget(unitsOfIter,3,2);

    setNumOfIter = new QPushButton("Set");

    pGB->addWidget(setNumOfIter,4,2);


    QLabel *lbl2;
    lbl2 = new QLabel("Graph Time Units:");
    pGB->addWidget(lbl2,1,3);

    graphUnitsBox = new QComboBox;

    graphUnitsBox->addItem("seconds");
    graphUnitsBox->addItem("minutes");
    graphUnitsBox->addItem("hours");
    graphUnitsBox->addItem("days");
    graphUnitsBox->addItem("years");

    pGB->addWidget(graphUnitsBox,2,3);

    setGraphUnits = new QPushButton("Set");

    pGB->addWidget(setGraphUnits,3,3);

    startComputations = new QPushButton("Start computations");
    connect(startComputations, SIGNAL(clicked()), SLOT(doComputations()));

    pGB->addWidget(startComputations,4,3,1,2);


    //table

    QLabel *lbl3;

    lbl3 = new QLabel("Current proccess preferences:");
    pGB->addWidget(lbl3,1,4);


    pgTable = new QTableWidget(3,2);

    QTableWidgetItem *row1pgtTitle;
    QTableWidgetItem *row2pgtTitle;
    QTableWidgetItem *row3pgtTitle;

    row1pgtTitle = new QTableWidgetItem("Time of single iteration",0);
    row2pgtTitle = new QTableWidgetItem("Number of iterations",0);
    row3pgtTitle = new QTableWidgetItem("Graph time Units",0);

    tableSingleIter = new QTableWidgetItem("--!--",0);
    tableNumOfIter = new QTableWidgetItem("-----",0);
    tableGraphUnits = new QTableWidgetItem("seconds",0);

    row1pgtTitle->setFlags(Qt::ItemIsEnabled);
    row2pgtTitle->setFlags(Qt::ItemIsEnabled);
    row3pgtTitle->setFlags(Qt::ItemIsEnabled);

    tableSingleIter->setFlags(Qt::ItemIsEnabled);
    tableNumOfIter->setFlags(Qt::ItemIsEnabled);
    tableGraphUnits->setFlags(Qt::ItemIsEnabled);

    pgTable->setItem(0,0,row1pgtTitle);
    pgTable->setItem(1,0,row2pgtTitle);
    pgTable->setItem(2,0,row3pgtTitle);

    pgTable->setItem(0,1,tableSingleIter);
    pgTable->setItem(1,1,tableNumOfIter);
    pgTable->setItem(2,1,tableGraphUnits);

    tableSingleIter->setText("-----");

    pgTable->verticalHeader()->hide();
    pgTable->horizontalHeader()->hide();
    pgTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pgTable->setColumnWidth(0,100);
    pgTable->setColumnWidth(1,300);


    pGB->addWidget(pgTable,2,4,2,1);


    pGB->setColumnStretch(1,4);
    pGB->setColumnStretch(2,3);
    pGB->setColumnStretch(3,3);
    pGB->setColumnStretch(4,9);
    pGB->setRowStretch(1,3);
    pGB->setRowStretch(2,2);
    pGB->setRowStretch(3,4);
    pGB->setRowStretch(4,5);
    pGB->setVerticalSpacing(10);

    connect(setTimeButton, SIGNAL(clicked()), this , SLOT(setSingleIterTime_fun()) );
    connect(setNumOfIter, SIGNAL(clicked()), this , SLOT(setNumOfIter_fun()) );
    connect(setGraphUnits, SIGNAL(clicked()), this , SLOT(setGraphUnits_fun()) );


    processGroupBox->setLayout(pGB);
}

/*!
    Добавляет в окно виджет для графика

 */
void radioactive::createPlot()
{
    plotWidget = new QWidget;
    mainChart = new QChart();
    mainChart->legend()->hide();
    mainChart->createDefaultAxes();
    mainChart->setTitle("Radioactivity of mix by time");
    mainChartView = new QChartView(mainChart);
    QGridLayout *chLayout = new QGridLayout;
    chLayout->addWidget(mainChartView);
    plotWidget->setLayout(chLayout);
}

/*!
    \brief Метод, добавляющий изотоп в смесь по нажатию соответствующей кнопки

    Вызывает в radioactivemix метод addIso()
 */
void radioactive::addButtonClicked()
{
    newLog("Add button Clicked");

    int i = isotopeToAdd->currentIndex();
    QString idI = isotopesList[i].first;
    QString mArg;
    mArg.append(idI[0]);mArg.append(idI[1]);mArg.append(idI[2]);
    int massArg = mArg.toInt();
    QString cArg;
    cArg.append(idI[4]);cArg.append(idI[5]);
    int chargeArg = cArg.toInt();

    bignumber quantityArg = bignumber(numOfMoleculesToAdd->toPlainText().toStdString());

    // id = mass + 0 + charge + 0|1|2(0 for now)
    theMix->addIso(isotope(massArg,chargeArg,quantityArg),theMix->isotope_list);

    refreshIsoTable();
}

/*!
    Переключение режимов между "полноэкранный в окне" и "окно"

 */
void radioactive::minimize_maximize()
{
    if(isMaximized())
    {
        showNormal();
        newLog("App minimized");
    }
    else
    {
        showMaximized();
        newLog("App maximized");
    }
}

/*!
    Выгружает из isotopedb все записи, предоставляя их интерфейсу

 */
void radioactive::getIsotopeList()
{
    isotDB = QSqlDatabase::addDatabase("QSQLITE");
    isotDB.setDatabaseName("isotopes.gdb");
    isotDB.open();
    newLog("Loading isotopes for list");
    QString request = "SELECT id,name FROM 'isotopes'";
    QSqlQuery isotQ(request,isotDB);
    QSqlRecord rec = isotQ.record();

    isotQ.first();
    QString cid = isotQ.value(0).toString();
    QString cname = isotQ.value(1).toString();
    newLog("Entry found in isotopes db: " + cid + " | " + cname);
    isotopesList.push_back(QPair <QString,QString> (cid,cname));

    while(isotQ.next())
    {
        QString cid = isotQ.value(0).toString();
        QString cname = isotQ.value(1).toString();
        newLog("Entry found in isotopes db: " + cid + " | " + cname);
        isotopesList.push_back(QPair <QString,QString> (cid,cname));
    }
    newLog("Isotopes db entries loaded successfully");


}

/*!
    Инициализирует модель базы isotopedb

 */
void radioactive::initializeModel()
{
    if(!createConnection())
    {
        newLog("FATAL! CREATECONNECTION FAILED!");
    }
    isotDB = QSqlDatabase::addDatabase("QSQLITE");
    isotDB.setDatabaseName("isotopes.gdb");
    isotDB.open();
    model = new QSqlTableModel(0,isotDB);

    model->setTable("isotopes");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    qDebug() << model->select();

    qDebug() << model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    qDebug() << model->setHeaderData(1, Qt::Horizontal, QObject::tr("Name"));
    qDebug() << model->setHeaderData(2, Qt::Horizontal, QObject::tr("Mass"));
    qDebug() << model->setHeaderData(3, Qt::Horizontal, QObject::tr("Charge"));
    qDebug() << model->setHeaderData(4, Qt::Horizontal, QObject::tr("Half-life period"));
    qDebug() << model->setHeaderData(5, Qt::Horizontal, QObject::tr("Alpha decay probability"));
    qDebug() << model->setHeaderData(6, Qt::Horizontal, QObject::tr("Beta decay probability"));
}

/*!
    Показывает таблицу изотопов из isotopedb

 */
void radioactive::showTable()
{
    QTableView *view = new QTableView;
    view->setModel(model);
    view->setWindowTitle("Isotopes");
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    newLog("Showing Isotopes DB");
    view->show();
}

/*!
    \brief Вызывает окно запроса к isotopedb

    Использует метод accessNeeded(), чтобы ограничить доступ к лишнему вмешательству в бд пользователю

 */
void radioactive::openConsoleDB()
{
    if(accessNeeded())
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setHostName("iSotopeServer");
        db.setDatabaseName("isotopes.gdb");
        db.open();
        QSqlQuery qr(db);
        QString request = QInputDialog::getText(0,"Isotopes DB","Enter request");
        if(qr.exec(request))
        {
            newLog("Request :''  "+request+"  '' successfully executed");
        }
        else
        {
            newLog("Request :''  "+request+"  '' failed to execute. Error: "+qr.lastError().text() );
        }

    }
    else
    {
        newLog("Wrong password entered!");
    }
}

/*!
    Метод, ограничивающий доступ к отдельным элементам программы

 */
bool radioactive::accessNeeded()
{
    if(unlocked)
    {
        return true;
    }
    QString trypass = QInputDialog::getText(0,"Insert password",
                                            "Защита от дурака, которого ты тут из себя строишь");
    qDebug() << trypass;
    newLog("Tried to enter password: " + trypass);
    if(trypass=="lockstock")
    {
        unlocked = true;
    }
    return unlocked;
}

/*!
    Открывает в отдельном окне лог текущего запуска приложения

 */
void radioactive::getMainLog()
{
    QString logg = "";
    for(int i=0;i<mainLog.size();i++)
    {
        logg+=mainLog[i]+"\n";
    }
    QMessageBox::information(0,"Log of current execution",logg);
}

void radioactive::setSingleIterTime_fun()
{
    singleIterTime = bignumber(timeOfIteration->toPlainText().toStdString());
    // units TBC

    tableSingleIter->setText(timeOfIteration->toPlainText() + " " + "seconds");
    // units TBC

}

void radioactive::setNumOfIter_fun()
{
    quantityOfIter = bignumber(numOfIter->toPlainText().toStdString());
    // units TBC

    tableNumOfIter->setText(numOfIter->toPlainText() + " " + "times");
    // units TBC
}

void radioactive::setGraphUnits_fun()
{
    tableGraphUnits->setText(graphUnitsBox->currentText());
}

void radioactive::refreshIsoTable()
{
    rmTable->clearContents();
    int j = rmTable->rowCount();
    for(int i=0;i<j;i++)
    {
        rmTable->removeRow(0);
    }
    newLog("Refreshing mix table. Num of isotopes now is:" + QString::number(theMix->isotope_list.size()));
    for(int i=0;i<theMix->isotope_list.size();i++)
    {
        newLog("Mix | " + theMix->isotope_list[i].name + ": " + QString::fromStdString(theMix->isotope_list[i].isoQuantity.ToString()));
    }

    for(int i=0;i<theMix->isotope_list.size();i++)
    {
            rmTable->insertRow(rmTable->rowCount());

            QTableWidgetItem *insName;
            QTableWidgetItem *insQua;


            std::string quaS;
            theMix->isotope_list[i].isoQuantity.ToString(quaS,10,false,100);

            QString quaSRounded = QString::fromStdString(quaS);

            quaSRounded = quaSRounded.split(".")[0];

            insName = new QTableWidgetItem(theMix->isotope_list[i].name);
            insQua = new QTableWidgetItem(quaSRounded);

            insName->setFlags(Qt::ItemIsEnabled);
            insQua->setFlags(Qt::ItemIsEnabled);

            rmTable->setItem(rmTable->rowCount()-1,0,insName);
            rmTable->setItem(rmTable->rowCount()-1,1,insQua);
    }
}

void radioactive::deleteAllRM_fun()
{
    rmTable->clearContents();
    int j = rmTable->rowCount();
    for(int i=0;i<j;i++)
    {
        rmTable->removeRow(0);
    }
    delete theMix;
    theMix = new radioactivemix;
}

void radioactive::deleteChosenRM_fun()
{
    int currow = rmTable->currentRow();
    if(currow==-1)
    {
        newLog("Deleting error. No row chosen");
        return;
    }

    newLog("Deleting chosen row (#" + QString::number(currow) + ")");
    for(int i=currow;i<theMix->isotope_list.size()-1;i++)
    {
        theMix->isotope_list[i] = theMix->isotope_list[i+1];
    }
    theMix->isotope_list.pop_back();
    refreshIsoTable();
}

void radioactive::doComputations()
{
    mixThread = new QThread;
    theMix->moveToThread(mixThread);
    statusLabel->setStyleSheet("QLabel { background-color : red; color : black; }");
    statusLabel->setText("Status: Computing...");
    newLog("Computations started");

    theMix->setDecayData(singleIterTime,quantityOfIter);

    connect(mixThread, SIGNAL(started()), theMix, SLOT(doNumOfDecays()));

    connect(theMix, SIGNAL(decaysFinished()), this, SLOT(computationsFinished()));

    mixThread->start();

}

void radioactive::computationsFinished()
{
    theMix->moveToThread(QThread::currentThread());
    mixThread->quit();
    mixThread->deleteLater();

    refreshIsoTable();
    newLog("Computations finished");

    statusLabel->setStyleSheet("QLabel { background-color : orange; color : black; }");
    statusLabel->setText("Status: Transfering data to graph...");

    //show graph here
    showGraph();

    statusLabel->setStyleSheet("QLabel { background-color : green; color : black; }");
    statusLabel->setText("Status: Ready");
}

void radioactive::showGraph()
{
    newLog("Making graph...");
    QFile readDots;
    readDots.setFileName("dots.txt");
    readDots.open(QIODevice::ReadOnly | QIODevice::Text);
    QLineSeries *ser = new QLineSeries();

    QTextStream inD(&readDots);
    while(!inD.atEnd())
    {
        QString nL = readDots.readLine();
        //newLog("Adding " + nL + " to series");
        ser->append(nL.split(" ")[0].toLongLong(),nL.split(" ")[1].toLongLong());
    }

    mainChart->createDefaultAxes();
    mainChart->addSeries(ser);


    //add axis to the chart
    QValueAxis *axisX = new QValueAxis;
    //axisX->setTickCount(10);
    axisX->setTitleText("Time");
    mainChart->addAxis(axisX, Qt::AlignBottom);
    ser->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    //axisY->setLabelFormat("%i");
    axisY->setTitleText("Activity");
    mainChart->addAxis(axisY, Qt::AlignLeft);
    ser->attachAxis(axisY);

    mainChartView->setRenderHint(QPainter::Antialiasing);

    newLog("Graph done?..");
}
