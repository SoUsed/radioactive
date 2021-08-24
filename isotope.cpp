#include "isotope.h"
#include "connectisotopedb.h"

#include <QDebug>

/*!
    \brief Класс isotope реализует модель некого количества радиоактивного изотопа, со всеми необходимыми характеристиками

    Выгружает общие данные о радиоакивном элементе из isotopedb
 */
isotope::isotope(QObject *parent, int mass, int charge, bignumber quantity) : QObject(parent)
{
    base2 = 2;

    _mass = mass;
    _charge = charge;
    isoQuantity = quantity;

    initModel();

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("iSotopeServer");
    db.setDatabaseName("isotopes.gdb");
    db.open();
    QSqlQuery qr(db);

    QString request = "SELECT 'Half-life period','Alpha decay probability','Beta decay probability' FROM 'isotopes' WHERE 'ID' = " + QString::number(mass) + "0" + QString::number(charge);

    QSqlRecord rec = qr.record();


    if(qr.exec())
    {
        _alpha_pr = qr.value(rec.indexOf("Alpha decay probability")).toFloat();
        _beta_pr = qr.value(rec.indexOf("Beta decay probability")).toFloat();
        _halflife = bignumber(qr.value(rec.indexOf("Half-life period")).toString().toStdString());
    }
    else
    {
        qDebug()<<"SQL FATAL | No Entry for current isotope in database ( "<<mass<<", " << charge << " )";
    }

}

/*!
    Инициализирует модель базы isotopedb

 */
void isotope::initModel()
{
    if(!createConnection())
    {
        qDebug("FATAL! CREATECONNECTION FAILED!");
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
    Проверяет, явлвяется ли данный элемент радиоактивным. Проводит поиск по бд isotopedb

 */
bool isotope::isIso(int mass, int charge)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("iSotopeServer");
    db.setDatabaseName("isotopes.gdb");
    db.open();
    QSqlQuery qr(db);

    QString request = "SELECT 'Name' FROM 'isotopes' WHERE 'ID' = " + QString::number(mass) + "0" + QString::number(charge);

    if(qr.exec()) //! или стоит проверять пуста ли запись? Просто само по себе выполнение никаких проблем не имеет, другое дело что результат никакой
    {
        return true;
    }
    return false;

}

/*!
    \brief Моделирует радиоактивный распад изотопа за промежуток iterTime

    Исользует формулу школьной физики N = N0 * 2^(t/T) для распада

    Рассматривает три сценарция:
    1) осуществить только бета распад, не пересчитвая распределение продуктов распада
    2) осуществить только альфа распад, не пересчитвая распределение продуктов распада
    3) осуществить оба распада, расчитывая распределение продуктов распада
    (кажется, что не приступать сразу к 3 в среднем экономней, ведь таких изотопов не то чтобы много.
    При этом, если оставить только 3, програма по функциональности не потеряет ничего)

 */
QPair < QVector<isotope> , bignumber> isotope::doDecays(bignumber iterTime)
{
    bignumber newQuantity = isoQuantity * base2.Pow(iterTime/_halflife);//! isoQuantity* 2 ^ (iterTime/_halflife)

    bignumber decN = isoQuantity - newQuantity;

    if(_beta_pr == 1)
    {
        isoQuantity = newQuantity;
        if(isIso(_mass,_charge+1))
        {
        QVector <isotope> vc;
        vc.push_back((isotope(0,_mass,_charge+1,decN)));
        return QPair < QVector<isotope>, bignumber> (vc , decN); // return new isotope
        }
        else
        {
            QVector <isotope> vc;
            return QPair < QVector<isotope>, bignumber> (vc , decN);
        }

    }
    else if(_alpha_pr == 1)
    {
        isoQuantity = newQuantity;
        if(isIso(_mass-4,_charge-2))
        {
        QVector <isotope> vc;
        vc.push_back(isotope(0,_mass-4,_charge-2,decN));
        return QPair < QVector<isotope>, bignumber> (vc , decN);  // return new isotope
        }
        else
        {
            QVector <isotope> vc;
            return QPair < QVector<isotope>, bignumber> (vc , decN);
        }
    }
    // также предусмотреть сценарий множественного распада
    else
    {
        isoQuantity = newQuantity;
        QVector <isotope> vc;
        if(isIso(_mass,_charge+1))
        {
        vc.push_back((isotope(0,_mass,_charge+1,decN*_alpha_pr)));
        }
        if(isIso(_mass-4,_charge-2))
        {
        vc.push_back(isotope(0,_mass-4,_charge-2,decN*_beta_pr));
        }
        return QPair < QVector<isotope>, bignumber> (vc , decN);
    }
}

/*!
    \brief Функция, которая добавляет в изотоп новые элементы

    Функцию использует преимущественно radioactivemix. Так же ее может использовать интерфейс, для добавления в смесь изотопов элемента, который там уже есть

    Верхние элементы радиоактивного ряда переходят в нижние во время каждой итерации.
    Использование этой функции необходимо для целосности isotope_list в radioactivemix, чтобы после итерации в списке не хранилось несколько одинаковых элементов
 */
void isotope::addQuantity(bignumber supplement)
{
    isoQuantity += supplement;
}
