#include "radioactivemix.h"

/*!
    \brief Класс с описанием радиоактивной смеси и ее поведения

    Взаимодействует с интерфейсом (получение информации о изменяемых в смеси даннных, ходе вычислений, получение сигналов об итерации)
    Является продвинутой реализацией списка элементов класса isotopes, реализуя более рациональные и последовательные операции со списком
    Взаимодействует с еще одной бд (точек) посредством метода addDot, передавая так данные в пользование python скрипту


 */
radioactivemix::radioactivemix(QObject *parent) : QObject(parent)
{
    currentlifetime=0;
}

/*!
    \brief Добавляет в смесь некоторое количество радиоактивного изотопа

    Сначала проверяет наличие такого изотопа в смеси уже, чтобы не инициализировать новый обьект класса isotope в случае чего
    Если такой изотоп уже есть, то используется метод addQuantity в уже имеющийся обьект класса, добавляя к нему переданное количество вещества
    Если же нет, то добавляет изотоп в список

    Используется:
    1) при вызове функции doDecays, для переноса элемнтов в новый список
    2) при формировании радиоактивной смеси посредством интерфейса


 */
void radioactivemix::addIso(isotope iso, QVector <isotope> & list)
{
    bool checker=false;
    for(int i=0;i<list.size();i++)
    {
        if(iso.getMC()==list[i].getMC())
        {
            list[i].addQuantity(iso.isoQuantity);
            checker=true;
        }
        if(checker)
        {
            break;
        }
    }
    if(!checker)
    {
        list.push_back(iso);
    }
}

/*!
    \brief Функция, которая реализует распад каждого из элементов радиоактивной смеси

    Записывает новую смесь (список) в отдельний список. После итерации новый список замещает старый
    Возвращает кол-во произведенных за итерацию распадов, для возможности последующей передачи в addDot()

 */
bignumber radioactivemix::doDecays(bignumber iterTime)
{
    QVector <isotope> newList;
    bignumber decays = 0;

    for(int i=0;i<isotope_list.size();i++)
    {
        QPair <QVector<isotope>, bignumber > decRes = isotope_list[i].doDecays(iterTime);
        decays += decRes.second;
        addIso(isotope_list[i],newList);
        for(int j=0;j<decRes.first.size();j++)
        {
            addIso(decRes.first[j],newList);
        }
    }
}

/*!
     Функция, которая записывает пару значений "кол-во распадов", "время" в бд, для последующей обработки, передачи в python скрипт и вывода на график по окончанию вычислений

 */
void radioactivemix::addDot(bignumber time, bignumber qua)
{
    //
    return;
}
