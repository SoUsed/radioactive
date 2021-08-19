#ifndef RADIOACTIVE_H
#define RADIOACTIVE_H

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


QT_BEGIN_NAMESPACE
namespace Ui { class radioactive; }
QT_END_NAMESPACE

class radioactive : public QWidget
{
    Q_OBJECT

public:
    radioactive(QWidget *parent = nullptr);
    ~radioactive();

    // logging
    std::vector <QString> mainLog;
    QFile log_to_save;
    void newLog(QString nLog);
    //

    //! Interface

    // main menu
    void createMenu();
    QMenu *fileMenu;
    QAction *exitAction;
    QAction *minimaxiAction;
    QMenuBar *menuBar;
    QMenu *logMenu;
    QAction *getLogAction_qDebug;
    QMenu *dbMenu;
    QAction *showIsotopesDataBase;
    QAction *openDBConsole;
    //

    // edit groupbox
    QGroupBox *editGroupBox;
    QTextEdit *numOfMoleculesToAdd;
    QComboBox *isotopeToAdd;

    std::vector <QAction> isotopesList;
    void getIsotopeList();

    void createEditGroup();
    QLabel *Edit1CollumnDesc;
    QPushButton *addButton;
    //


    // process groupbox
    QGroupBox *processGroupBox;
    QTextEdit *timeOfIteration;
    void createProcessGroup();

    QLabel *Process1CollumnDesc;

    QComboBox *unitsIteration;
    QAction *secondsIteration;
    QAction *minutesIteration;
    QAction *hoursIteration;
    QAction *yearsIteration;

    //! !?
    QPushButton *continueButton;
    QPushButton *stopButton;
    QPushButton *setTimeButton;
    //!

    QTextEdit *updateTime;
    QComboBox *unitsUpdate;
    QLabel *Process2CollumnDesc;
    QPushButton *setUpdateButton;

    QAction *secondsUpdate;
    QAction *minutesUpdate;
    QAction *hoursUpdate;
    QAction *yearsUpdate;

    QLabel *Process4CollumnDesc;
    QPushButton *stopIt;
    QPushButton *continueIt;
    QComboBox *graphUnitsBox;
    QPushButton *setGraphUnits;
    QLabel *pauseunpause;

    QAction *secondsGraph;
    QAction *minutesGraph;
    QAction *hoursGraph;
    QAction *yearsGraph;
    //

    // Plot

    QWidget *plotWidget;
    void createPlot();
    QChart *mainChart;
    QChartView *mainChartView;

    //!



    //SQL
    QSqlDatabase isotDB;
    QSqlTableModel *model;
    void initializeModel();


    QVector <QString> isotopesInfo;

    //

    //prv
    bool accessNeeded();
    bool unlocked;


private:
    Ui::radioactive *ui;

public slots:
    void minimize_maximize();

    //void getMainLog_qDebug();

    void addButtonClicked();
    //SQL
    void showTable();
    void openConsoleDB();
};
#endif // RADIOACTIVE_H
