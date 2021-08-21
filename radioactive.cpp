#include "radioactive.h"
#include "ui_radioactive.h"

#include "connectisotopedb.h"

#include <QTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QLayout>

#include <QMessageBox>

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



}

radioactive::~radioactive()
{
    //getMainLog();
    delete ui;
}

void radioactive::newLog(QString nLog)
{
    QString logToPush = "[" + QTime::currentTime().toString("HH:mm:ss") + "] " +  nLog;
    mainLog.push_back(logToPush);
    qDebug() << QString(logToPush);
    QTextStream log_qts(&log_to_save);
    log_qts<<logToPush << "\n";

}

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
    //getIsotopeList();
    isotopeToAdd->setCurrentText("Choose Isotope");

    eGB->addWidget(isotopeToAdd,3,1);

    addButton = new QPushButton("Add");
    connect(addButton,SIGNAL(clicked()),this,SLOT(addButtonClicked()));
    eGB->addWidget(addButton,4,1);



    eGB->setColumnStretch(1,4);
    eGB->setColumnStretch(2,3);
    eGB->setColumnStretch(3,11);
    eGB->setRowStretch(1,3);
    eGB->setRowStretch(2,4);
    eGB->setRowStretch(3,4);
    eGB->setRowStretch(4,5);

    editGroupBox->setLayout(eGB);

}

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
    pGB->addWidget(unitsIteration,3,1);


    setTimeButton = new QPushButton("Set");
    pGB->addWidget(setTimeButton,4,1);


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
    pGB->addWidget(Process4CollumnDesc,1,4);

    graphUnitsBox = new QComboBox;
    pGB->addWidget(graphUnitsBox,2,4);

    setGraphUnits = new QPushButton("Set");
    pGB->addWidget(setGraphUnits,3,4);

    QHBoxLayout *prL = new QHBoxLayout;

    pauseunpause = new QLabel("Process:");
    prL->addWidget(pauseunpause,4);

    continueIt = new QPushButton("Continue/start");
    prL->addWidget(continueIt,5);

    stopIt = new QPushButton("Stop");
    prL->addWidget(stopIt,3);

    pGB->addLayout(prL,4,4);


    pGB->setColumnStretch(1,4);
    pGB->setColumnStretch(2,3);
    pGB->setColumnStretch(3,9);
    pGB->setColumnStretch(4,2);
    pGB->setRowStretch(1,3);
    pGB->setRowStretch(2,2);
    pGB->setRowStretch(3,4);
    pGB->setRowStretch(4,5);
    pGB->setVerticalSpacing(10);

    processGroupBox->setLayout(pGB);
}

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

void radioactive::addButtonClicked()
{
    newLog("Add button Clicked");
}

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

void radioactive::getIsotopeList()
{
    isotDB = QSqlDatabase::addDatabase("QSQLITE");
    isotDB.setDatabaseName("isotopes.gdb");
    isotDB.open();
    QSqlQuery isotQ(isotDB);
    isotQ.exec("SELECT * FROM isotopes");
    QString bfs;
    while(true)
    {
        //TBAF
        break;
    }

}

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

void radioactive::showTable()
{
    QTableView *view = new QTableView;
    view->setModel(model);
    view->setWindowTitle("Isotopes");
    newLog("Showing Isotopes DB");
    view->show();
}

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

void radioactive::getMainLog()
{
    QString logg = "";
    for(int i=0;i<mainLog.size();i++)
    {
        logg+=mainLog[i]+"\n";
    }
    QMessageBox::information(0,"Log of current execution",logg);
}
