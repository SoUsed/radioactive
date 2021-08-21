#include "isotope.h"
#include "connectisotopedb.h"

isotope::isotope(QObject *parent, int mass, int charge, bignumber quantity) : QObject(parent)
{
    _mass = mass;
    _charge = charge;
    isoQuantity = quantity;

    initModel();
    //! вот тут надо выгребать остаток штук из бд, ану допили потом
}

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

void isotope::doDecays(int iterTime)
{
    bignumber newQuantity = 0;//! isoQuantity* "e" ^ (iterTime/_halflife)

    bignumber decN = isoQuantity - newQuantity;

    if(_beta_pr == 1)
    {
        isotope newIso = isotope(nullptr,_mass,_charge+1,decN);
        isoQuantity = newQuantity;
        return; // return new isotope?
    }
    else if(_alpha_pr == 1)
    {
        isotope newIso = isotope(nullptr,_mass-4,_charge-2,decN);
        isoQuantity = newQuantity;
        return; // return isotope?
    }
    // также предусмотреть сценарий множественного распада
}

void isotope::addQuantity(bignumber supplement)
{
    isoQuantity += supplement;
}
