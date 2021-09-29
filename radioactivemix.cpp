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
    dots_to_save.setFileName("dots.txt");
    dots_to_save.open(QIODevice::ReadWrite | QIODevice::Text);
    dots_to_save.remove();
    dots_to_save.open(QIODevice::ReadWrite | QIODevice::Text);
}

/*!
    \brief Добавляет в смесь некоторое количество радиоактивного изотопа

    Сначала проверяет наличие такого изотопа в смеси уже, чтобы не инициализировать новый обьект класса isotope в случае чего
    Если такой изотоп уже есть, то используется метод addQuantity в уже имеющийся обьект класса, добавляя к нему переданное количество вещества
    Если же нет, то добавляет изотоп в список

    Используется при формировании радиоактивной смеси посредством интерфейса

    Оперирует непосредственно с обьектом класса isotope

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
    \brief Добавляет в смесь некоторое количество радиоактивного изотопа

    Сначала проверяет наличие такого изотопа в смеси уже, чтобы не инициализировать новый обьект класса isotope в случае чего
    Если такой изотоп уже есть, то используется метод addQuantity в уже имеющийся обьект класса, добавляя к нему переданное количество вещества
    Если же нет, то добавляет изотоп в список

    Используется при вызове функции doDecays, для переноса элемнтов в новый список

    Оперирует не с обьектом класса isotope, а с минимальным необходимым набором данных о изотопе. Предпологая, что чаще в смеси уже будет добавляемый изотоп,
    данный метод не создает новый обьект класса isotope до последного, ведь создание обьекта намного медленнее, чем создание equalIsoData внутри doDecays
 */
void radioactivemix::addIso(equalIsoData isoData, QVector <isotope> & list)
{
    bool checker=false;
    for(int i=0;i<list.size();i++)
    {
        if(isoData.first==list[i].getMC())
        {
            list[i].addQuantity(isoData.second);
            checker=true;
        }
        if(checker)
        {
            break;
        }
    }
    if(!checker)
    {
        list.push_back(isotope(isoData.first.first,isoData.first.second,isoData.second));
    }
}

/*!
    \brief Функция, которая реализует распад каждого из элементов радиоактивной смеси

    Записывает новую смесь (список) в отдельний список. После итерации новый список замещает старый
    Возвращает кол-во произведенных за итерацию распадов, для возможности последующей передачи в addDot()

 */
void radioactivemix::doDecays(bignumber iterTime)
{
    QVector <isotope> newList;
    bignumber decays = 0;

    for(int i=0;i<isotope_list.size();i++)
    {
        QPair <QVector<equalIsoData>, bignumber > decRes = isotope_list[i].doDecays(iterTime);
        decays += decRes.second;
        addIso(isotope_list[i],newList);
        for(int j=0;j<decRes.first.size();j++)
        {
            addIso(decRes.first[j],newList);
        }
    }
    isotope_list = newList;
    currentlifetime+=iterTime;
    addDot(currentlifetime,decays);
}

/*!
     Функция, которая записывает пару значений "кол-во распадов", "время" в файл, для последующей обработки, передачи в python скрипт и вывода на график по окончанию вычислений

 */
void radioactivemix::addDot(bignumber time, bignumber qua)
{
    QTextStream dots_qts(&dots_to_save);
    QString quaSRounded = QString::fromStdString(qua.ToString());
    quaSRounded = quaSRounded.split(".")[0];
    dots_qts << QString::fromStdString(time.ToString()) + " " + quaSRounded << "\n";
    return;
}

/*!
    Функция, которая проводит распады для радиоактивной смеси.
    Является слотом на сигнал о начале работы потока для вычислений

 */
void radioactivemix::doNumOfDecays()
{
    doDo=true;
    if(!createConnection())
    {
        qDebug()<< "FATAL! CREATECONNECTION FAILED!";
    }

    QSqlQuery query1(db);
    query1.exec("select group_concat(id, ',') from isotopes");
    query1.first();
    isotope::dbIds = query1.value(0).toString().split(",");
    qDebug()<<"TheMix | computing!";
    ticker=0;
    while(ticker<quantityOfIter)
    {
        if(!doDo)
        {
            break;
        }
        doDecays(iterTime);
        ticker++;
    }
    this->moveToThread(mainThread);
    emit decaysFinished();
}

/*!
    Передает параметры о будущих вычислениях в смесь из интерфейса. Вызов предшествует вызову doNumOfDecays.
    Нужна просто, чтобы ничего не тянуть в doNumOfDecays

 */
void radioactivemix::setDecayData(bignumber timeOfIter, bignumber iterquantity)
{
    iterTime = timeOfIter;
    quantityOfIter = iterquantity;
}
