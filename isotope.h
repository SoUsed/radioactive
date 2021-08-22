#ifndef ISOTOPE_H
#define ISOTOPE_H

#include <QObject>
#include <QtSql>
#include <QDebug>

#include "ttmath/ttmath.h"

typedef ttmath::Big<TTMATH_BITS(64), TTMATH_BITS(256)> bignumber;


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

    isotope doDecays(bignumber iterTime=0);

    void addQuantity(bignumber supplement=0);

    bool isIso(int mass, int charge);


signals:

};

#endif // ISOTOPE_H
