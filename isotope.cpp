#include "isotope.h"
#include "connectisotopedb.h"

#include <QDebug>

isotope::isotope(QObject *parent, int mass, int charge, bignumber quantity) : QObject(parent)
{
    _mass = mass;
    _charge = charge;
    isoQuantity = quantity;

    initModel();
    //! вот тут надо выгребать остаток штук из бд, ану допили потом
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

isotope isotope::doDecays(bignumber iterTime)
{
    bignumber newQuantity = isoQuantity * ttmath::Exp(iterTime/_halflife);//! isoQuantity* "e" ^ (iterTime/_halflife)

    bignumber decN = isoQuantity - newQuantity;

    if(_beta_pr == 1)
    {
        isoQuantity = newQuantity;
        if(isIso(_mass,_charge+1))
        {
        return isotope(0,_mass,_charge+1,decN); // return new isotope
        }

    }
    else if(_alpha_pr == 1)
    {
        isoQuantity = newQuantity;
        if(isIso(_mass-4,_charge-2))
        {
        return isotope(0,_mass-4,_charge-2,decN); // return new isotope
        }
    }
    // также предусмотреть сценарий множественного распада
}

void isotope::addQuantity(bignumber supplement)
{
    isoQuantity += supplement;
}
