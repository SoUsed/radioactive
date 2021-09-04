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
#include "ttmath/ttmath.h"

#include "radioactivemix.h"

typedef ttmath::Big<TTMATH_BITS(64), TTMATH_BITS(256)> bignumber;

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

    //! переменная, в которой хранятся все логи текущего выполнения программы
    std::vector <QString> mainLog;

    //! файл, в который последовательно записывается mainLog. С нуля ерезаписывается при новом запуске программы
    QFile log_to_save;
    void newLog(QString nLog);
    //

    /// Interface

    // main menu
    void createMenu();
    QMenu *fileMenu;
    QAction *exitAction;
    QAction *minimaxiAction;
    QMenuBar *menuBar;
    QMenu *logMenu;
    QAction *getLogAction;
    QMenu *dbMenu;
    QAction *showIsotopesDataBase;
    QAction *openDBConsole;
    //

    // edit groupbox
    QGroupBox *editGroupBox;

    //! рамка для ввода количества добавляемого в смесь вещества
    QTextEdit *numOfMoleculesToAdd;
    QComboBox *isotopeToAdd;
    QComboBox *unitsToAdd;

    //! список всех изотопов, используемый для выпадающего списка isotopeToAdd. Пара значений: id,name
    std::vector < QPair<QString,QString> > isotopesList;
    void getIsotopeList();

    void createEditGroup();
    QLabel *Edit1CollumnDesc;
    QPushButton *addButton;
    //


    // process groupbox
    QGroupBox *processGroupBox;

    //! рамка для ввода времени одной итерации
    QTextEdit *timeOfIteration;
    void createProcessGroup();

    QLabel *Process1CollumnDesc;

    //! выпадающий список, в котором быбираются единицы времени для timeOfIteration
    QComboBox *unitsIteration;
    QPushButton *setTimeButton;


    //! рамка для ввода количества итераций
    QTextEdit *numOfIter;
    QComboBox *unitsOfIter;
    QPushButton *setNumOfIter;

    //QAction *secondsIteration;
    //QAction *minutesIteration;
    //QAction *hoursIteration;
    //QAction *yearsIteration;

    /*
    //! !?
    QPushButton *continueButton;
    //! !?
    QPushButton *stopButton;
    //! !?

    //!

    //! !?
    QTextEdit *updateTime;
    //! !?
    QComboBox *unitsUpdate;
    //! !?
    QLabel *Process2CollumnDesc;
    //! !?
    QPushButton *setUpdateButton;

    //! !?
    QAction *secondsUpdate;
    //! !?
    QAction *minutesUpdate;
    //! !?
    QAction *hoursUpdate;
    //! !?
    QAction *yearsUpdate;

    QLabel *Process4CollumnDesc;
    //! !?
    QPushButton *stopIt;
    //! !?
    QPushButton *continueIt;
    */

    //! Табличка, отображающая текущую смесь
    QTableWidget *rmTable;
    void refreshIsoTable();

    QPushButton *deleteAllRM;
    QPushButton *deleteChosenRM;

    QLabel *statusLabel;

    //! выпадающий список для выбора единиц времени на оси на графике (с,мин,ч,год)
    QComboBox *graphUnitsBox;
    QPushButton *setGraphUnits;
    QPushButton *startComputations;

    //QAction *secondsGraph;
    //QAction *minutesGraph;
    //QAction *hoursGraph;
    //QAction *yearsGraph;
    //

    QTableWidget *pgTable;
    QTableWidgetItem *tableSingleIter;
    QTableWidgetItem *tableNumOfIter;
    QTableWidgetItem *tableGraphUnits;

    bignumber singleIterTime;
    bignumber quantityOfIter;

    // Plot

    //! виджет, в котором должно в теории вмещаться окно интерфейса cufflinks с графиком
    QWidget *plotWidget;
    void createPlot();
    QChart *mainChart;
    QChartView *mainChartView;

    //!



    //SQL

    //! переменная базы данных isotopedb
    QSqlDatabase isotDB;

    //! модель бд isotopedb
    QSqlTableModel *model;
    void initializeModel();


    QVector <QString> isotopesInfo;

    //

    //prv
    bool accessNeeded();

    //! переменная, которая хранит информацию о том, ввел ли пользователь (является ли он авторизованным)
    bool unlocked;

    //! обьект radioactivemix, над которым будут проводится все действия
    radioactivemix *theMix;

private:
    Ui::radioactive *ui;

public slots:
    void minimize_maximize();

    void getMainLog();

    void addButtonClicked();
    //SQL
    void showTable();
    void openConsoleDB();

    //slots 4 processGroupBox signals
    void setSingleIterTime_fun();
    void setNumOfIter_fun();
    void setGraphUnits_fun();

    //mix operations
    void deleteAllRM_fun();
    void deleteChosenRM_fun();

    //comp
    void doComputations();

};
#endif // RADIOACTIVE_H
