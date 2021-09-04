#ifndef RADIOACTIVEMIX_H
#define RADIOACTIVEMIX_H

#include <QObject>
#include <isotope.h>

class radioactivemix : public QObject
{
    Q_OBJECT

    //! текущее время жизни смеси. Используется для построения графика от времени
    bignumber currentlifetime;

public:
    explicit radioactivemix(QObject *parent = nullptr);

    //! основная информация о радиоактивной смеси - её составляющие
    QVector <isotope> isotope_list;

    void addIso(isotope iso, QVector <isotope> & list);

    bignumber doDecays(bignumber iterTime=0);

    void addDot(bignumber time, bignumber qua);

    QFile dots_to_save;


signals:

};

#endif // RADIOACTIVEMIX_H
