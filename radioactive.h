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
#include <QTimer>

#include "ttmath/ttmath.h"

#include "radioactivemix.h"

typedef ttmath::Big<TTMATH_BITS(64), TTMATH_BITS(256)> bignumber;

QT_BEGIN_NAMESPACE
namespace Ui { class radioactive; }
QT_END_NAMESPACE

class radioactive : public QWidget
{
    Q_OBJECT

    Ui::radioactive *ui;

    //! поток, в который отделяется theMix на время вычислений
    QThread *mixThread;

    // переменные, используемые для отслеживания скорости и прогресса вычислений
    int timeElapsed;
    int currentTick;
    QTimer compTimer;
    bool localFlagTimer;

public:     
    radioactive(QWidget *parent = nullptr);
    ~radioactive();

    //----- logging

    //! переменная, в которой хранятся все логи текущего выполнения программы
    std::vector <QString> mainLog;

    //! файл, в который последовательно записывается mainLog. С нуля ерезаписывается при новом запуске программы
    QFile log_to_save;
    void newLog(QString nLog);
    //-----

    //---------- Interface
    QVBoxLayout *mainLayout;

    //----- main menu
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
    //-----

    //----- edit groupbox
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

    //! кнопка добавления выбраного изотопа в theMix
    QPushButton *addButton;
    //-----

    //----- process groupbox
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

    //! Табличка, отображающая текущую смесь
    QTableWidget *rmTable;
    void refreshIsoTable();

    //! Кнопка справа от табл.смеси, очищает theMix
    QPushButton *deleteAllRM;
    //! Кнопка справа от табл.смеси, убирает из theMix выбраный в таблице элемент
    QPushButton *deleteChosenRM;
    //! Кнопка справа от табл.смеси. Вызывает abortComputations()
    QPushButton *stopIter;

    //! поле для статистики вычислений (прогресс, скорость, средняя скорость)
    QLabel *computationsStatsLabel;

    //! поле для статуса програмы (готова, идут вычисления, рисуется график)
    QLabel *statusLabel;

    //! выпадающий список для выбора единиц времени на оси на графике (с,мин,ч,год)
    QComboBox *graphUnitsBox;
    QPushButton *setGraphUnits;

    QPushButton *startComputations;

    //! Таблица, в поля tableSingleIter, tableNumOfIter, tableGraphUnits которой записываются параметры вычислений
    QTableWidget *pgTable;
    QTableWidgetItem *tableSingleIter;
    QTableWidgetItem *tableNumOfIter;
    QTableWidgetItem *tableGraphUnits;
    //-----

    //----- Plot
    //! виджет, в котором должно в теории вмещаться окно интерфейса cufflinks с графиком
    QWidget *plotWidget;
    void createPlot();
    QChart *mainChart;
    QChartView *mainChartView;
    //-----

    //-----SQL
    //! переменная базы данных isotopedb
    QSqlDatabase isotDB;
    //! модель бд isotopedb
    QSqlTableModel *model;
    void initializeModel();
    //-----

    //----- security
    bool accessNeeded();

    //! переменная, которая хранит информацию о том, ввел ли пользователь (является ли он авторизованным)
    bool unlocked;
    //-----

    //----------

    //! обьект radioactivemix, над которым будут проводится все действия
    radioactivemix *theMix;

    bool programmReadiness();

    void showGraph();

    void getGraphUnitsDiv();

    //! переменнная, используемая для того, чтобы отобразить время на графике в нужных единицах
    int graphUnitsDivider;

    //! переменная, передаваемая в theMix. Условное время одной итерации в секундах
    bignumber singleIterTime;
    //! переменная, передаваемая в theMix. Количество итераций
    bignumber quantityOfIter;

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
    void computationsFinished();

    void abortComputations();

    //timer
    void updateSpeedStats();
};
#endif // RADIOACTIVE_H
