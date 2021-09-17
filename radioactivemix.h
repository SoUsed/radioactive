#ifndef RADIOACTIVEMIX_H
#define RADIOACTIVEMIX_H

#include <QObject>
#include <isotope.h>

class radioactivemix : public QObject
{
    Q_OBJECT

    //! текущее время жизни смеси. Используется для построения графика от времени
    bignumber currentlifetime;

    //
    bignumber iterTime;
    bignumber quantityOfIter;

public:
    explicit radioactivemix(QObject *parent = nullptr);

    //! основная информация о радиоактивной смеси - её составляющие
    QVector <isotope> isotope_list;

    void addIso(isotope iso, QVector <isotope> & list);

    void doDecays(bignumber iterTime=0);

    void addDot(bignumber time, bignumber qua);

    QFile dots_to_save;

    void setDecayData(bignumber timeOfIter=0,bignumber iterquantity=0);

    QSqlDatabase db;

    //! Флажок для прерывания вычислений из интерфейса
    bool doDo;

    //! переменная, в которую записывается количество проделанных операций. Нужна для отслеживания скорости вычислений
    bignumber ticker;

signals:
    void decaysFinished();

public slots:
    void doNumOfDecays();

};

#endif // RADIOACTIVEMIX_H
