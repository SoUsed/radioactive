#ifndef CONNECTISOTOPEDB_H
#define CONNECTISOTOPEDB_H

#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>
static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("iSotopeServer");
    //db.setDatabaseName("D:/Vova/radioactive/isotopes.gdb");
    db.setDatabaseName("isotopes.gdb");
    if (!db.open()) {
        QMessageBox::critical(0, "Cannot open database",
            "Unable to establish a database connection.\n"
                     "This example needs SQLite support. Please read "
                     "the Qt SQL driver documentation for information how "
                     "to build it.\n\n"
                     "Click Cancel to exit.", QMessageBox::Cancel);
        return false;
    }

    QSqlQuery query(db);
    qDebug() << query.exec("create table isotopes (id int primary key, "
               "name varchar(20), mass int, charge int, halflife varchar(25), "
               "alphaProb varchar(12), betaProb varchar(12) )");
    /*
    if (!query.execBatch())
        qDebug() << "LE: " << query.lastError();
    qDebug() << query.exec("insert into isotopes values (2380920, 'Uranium-238', 238, 92, '141005809965893500','1','0')");
    if (!query.execBatch())
        qDebug() << "LE: " << query.lastError();
    qDebug() << query.exec("insert into isotopes values (2340900, 'Thorium-234', 234, 90, '2082240','0','1')");
    if (!query.execBatch())
        qDebug() << "LE: " << query.lastError();
    qDebug() << query.exec("insert into isotopes values (2340910, 'Protactinium-234M', 234, 91, '69.6','0','0.9984')");
    if (!query.execBatch())
        qDebug() << "LE: " << query.lastError();
    qDebug() << query.exec("insert into isotopes values (2340911, 'Protactinium-234', 234, 91, '24120','0','1')");
    if (!query.execBatch())
        qDebug() << "LE: " << query.lastError();
    qDebug() << query.exec("insert into isotopes values (2340920, 'Uranium-234', 234, 92, '7731446734025','1','0')");
    if (!query.execBatch())
        qDebug() << "LE: " << query.lastError();

    //to add some elements u need to open database editor(menu or F3) and write:
    //insert into isotopes values (id, 'name', mass, charge, 'half-life(sec)','alpha prob(0-1)','beta prob(0-1)')
    */
    return true;
}
#endif // CONNECTISOTOPEDB_H
