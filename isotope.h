#ifndef ISOTOPE_H
#define ISOTOPE_H

#include <QObject>
#include <QtSql>
#include <QDebug>

#include "ttmath/ttmath.h"
#include "connectisotopedb.h"

typedef ttmath::Big<TTMATH_BITS(64), TTMATH_BITS(256)> bignumber;


class isotope
{

    // data to be got from db
    //! Заряд ядра. В паре с массой является уникальным идентификатором
    int _charge;

    //! Масса атома в а.о.м. В паре с зарядом является уникальным идентификатором
    int _mass;

    //! Табличное значение: вероятность того, что нестабильный элемент совершит альфа-распад
    double _alpha_pr;

    //! Табличное значение: вероятность того, что нестабильный элемент совершит бета-распад
    double _beta_pr;

    //! Табличное значение: период полураспада
    bignumber _halflife;
    //





public:

    explicit isotope(int mass=0, int charge = 0, bignumber quantity=0);

    //! обьект isotope так же хранит кол-во своих атомов в смеси
    bignumber isoQuantity;

    //! Название для таблицы
    QString name;

    //! переменная для бд isotopedb
    QSqlDatabase db;
    //QSqlQuery qr;

    //! модель бд isotopedb
    QSqlTableModel *model;
    void initModel();

    QPair < QVector<isotope> , bignumber> doDecays(bignumber iterTime=0);

    void addQuantity(bignumber supplement=0);

    bool isIso(int mass, int charge);

    //! возвращает пару значений "масса, заряд" для сравнения изотопов внутри смеси
    QPair<int,int> getMC()
    {
        return QPair<int,int>(_mass,_charge);
    }


signals:

};

#endif // ISOTOPE_H
