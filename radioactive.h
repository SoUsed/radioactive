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
    std::vector <QString> mainLog; /// переменная, в которой хранятся все логи текущего выполнения программы
    QFile log_to_save; /// файл, в который последовательно записывается mainLog. С нуля ерезаписывается при новом запуске программы
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
    QAction *getLogAction;
    QMenu *dbMenu;
    QAction *showIsotopesDataBase;
    QAction *openDBConsole;
    //

    // edit groupbox
    QGroupBox *editGroupBox;
    QTextEdit *numOfMoleculesToAdd; /// рамка для ввода количества добавляемого в смесь вещества
    QComboBox *isotopeToAdd;

    std::vector <QAction> isotopesList; /// список всех изотопов, используемый для выпадающего списка isotopeToAdd
    void getIsotopeList();

    void createEditGroup();
    QLabel *Edit1CollumnDesc;
    QPushButton *addButton;
    //


    // process groupbox
    QGroupBox *processGroupBox;
    QTextEdit *timeOfIteration; /// рамка для ввода времени одной итерации
    void createProcessGroup();

    QLabel *Process1CollumnDesc;

    QComboBox *unitsIteration; /// выпадающий список, в котором быбираются единицы времени для timeOfIteration
    QAction *secondsIteration;
    QAction *minutesIteration;
    QAction *hoursIteration;
    QAction *yearsIteration;

    //! !?
    QPushButton *continueButton;
    //! !?
    QPushButton *stopButton;
    //! !?
    QPushButton *setTimeButton;
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
    QComboBox *graphUnitsBox; /// выпадающий список для выбора единиц времени на оси на графике (с,мин,ч,год)
    QPushButton *setGraphUnits;
    QLabel *pauseunpause;

    QAction *secondsGraph;
    QAction *minutesGraph;
    QAction *hoursGraph;
    QAction *yearsGraph;
    //

    // Plot

    QWidget *plotWidget; /// виджет, в котором должно в теории вмещаться окно интерфейса cufflinks с графиком
    void createPlot();
    QChart *mainChart;
    QChartView *mainChartView;

    //!



    //SQL
    QSqlDatabase isotDB; /// переменная базы данных isotopedb
    QSqlTableModel *model; /// модель бд isotopedb
    void initializeModel();


    QVector <QString> isotopesInfo;

    //

    //prv
    bool accessNeeded();
    bool unlocked; /// переменная, которая хранит информацию о том, ввел ли пользователь (является ли он авторизованным)


private:
    Ui::radioactive *ui;

public slots:
    void minimize_maximize();

    void getMainLog();

    void addButtonClicked();
    //SQL
    void showTable();
    void openConsoleDB();
};
#endif // RADIOACTIVE_H
