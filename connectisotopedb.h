#ifndef CONNECTISOTOPEDB_H
#define CONNECTISOTOPEDB_H

#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>
static QVector<QString> dbIds; // список

//to add some elements u need to open database editor(menu or F3) and write:
//insert into isotopes values (id, 'name', mass, charge, 'half-life(sec)','alpha prob(0-1)','beta prob(0-1)')

static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("iSotopeServer");
    db.setDatabaseName("isotopes.gdb");
    if (!db.open()) {
        QMessageBox::critical(0, "Cannot open database",
            "Unable to load isotopes.gdb", QMessageBox::Cancel);
        return false;
    }
    return true;
}
#endif // CONNECTISOTOPEDB_H
