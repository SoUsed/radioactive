#ifndef ISOTOPE_H
#define ISOTOPE_H

#include <QObject>
#include <libs/floatx.hpp>
#include <QtSql>
#include <QDebug>

typedef flx::floatx<64,256> bignumber;

class isotope : public QObject
{
    Q_OBJECT

    // data to be got from db
    int _charge;
    int _mass;
    double _alpha_pr;
    double _beta_pr;
    bignumber _halflife;
    //





public:
    explicit isotope(QObject *parent = nullptr, int mass=0, int charge = 0, bignumber quantity=0);

    bignumber isoQuantity;

    QSqlDatabase isotDB;
    QSqlTableModel *model;
    void initModel();

    void doDecays(int iterTime=0);

    void addQuantity(bignumber supplement=0);


signals:

};

#endif // ISOTOPE_H
