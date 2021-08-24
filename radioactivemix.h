#ifndef RADIOACTIVEMIX_H
#define RADIOACTIVEMIX_H

#include <QObject>
#include <isotope.h>

class radioactivemix : public QObject
{
    Q_OBJECT

    bignumber currentlifetime; /// текущее время жизни смеси. Используется для построения графика от времени

public:
    explicit radioactivemix(QObject *parent = nullptr);

    QVector <isotope> isotope_list; /// основная информация о радиоактивной смеси - её составляющие

    void addIso(isotope iso, QVector <isotope> & list);

    bignumber doDecays(bignumber iterTime=0);

    void addDot(bignumber time, bignumber qua);


signals:

};

#endif // RADIOACTIVEMIX_H
