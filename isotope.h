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
    int _charge; /// Заряд ядра. В паре с массой является уникальным идентификатором
    int _mass;  /// Масса атома в а.о.м. В паре с зарядом является уникальным идентификатором
    double _alpha_pr; /// Табличное значение: вероятность того, что нестабильный элемент совершит альфа-распад
    double _beta_pr; /// Табличное значение: вероятность того, что нестабильный элемент совершит бета-распад
    bignumber _halflife; /// Табличное значение: период полураспада
    //





public:
    const bignumber base2=2; /// Число 2, но просто записанное в bignumber. Нужна, чтобы совершать операцию 2^bignumber

    explicit isotope(QObject *parent = nullptr, int mass=0, int charge = 0, bignumber quantity=0);

    bignumber isoQuantity; /// обьект isotope так же хранит кол-во своих атомов в смеси

    QSqlDatabase isotDB; /// переменная для бд isotopedb
    QSqlTableModel *model; /// модель бд isotopedb
    void initModel();

    QPair < QVector<isotope> , bignumber> doDecays(bignumber iterTime=0);

    void addQuantity(bignumber supplement=0);

    bool isIso(int mass, int charge);

    QPair<int,int> getMC() /// возвращает пару значений "масса, заряд" для сравнения изотопов внутри смеси
    {
        return QPair<int,int>(_mass,_charge);
    }


signals:

};

#endif // ISOTOPE_H
