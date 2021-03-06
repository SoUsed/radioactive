#include "isotope.h"

#include <QDebug>

QVector<QString> isotope::dbIds;

/*!
    \brief Класс isotope реализует модель некого количества радиоактивного изотопа, со всеми необходимыми характеристиками

    Выгружает общие данные о радиоакивном элементе из isotopedb
 */
isotope::isotope(int mass, int charge, bignumber quantity)
{
    _mass = mass;
    _charge = charge;
    isoQuantity = quantity;

    QString idStr = QString( QString::number(mass) + "0" + QString::number(charge) + "0"); // формируем id изотопа в бд (определено массой и зарядом)
    QString request = "SELECT halflife,alphaProb,betaProb,name FROM 'isotopes' WHERE id = " + idStr;
    QSqlQuery qr(request,db);
    if(qr.first())
    {
        _alpha_pr = qr.value(1).toFloat();
        _beta_pr = qr.value(2).toFloat();
        _halflife = bignumber(qr.value(0).toString().toStdString());
        name = qr.value(3).toString();
    }
    else
    {
        qDebug()<<"SQL FATAL | No Entry for current isotope in database ( "<<mass<<", " << charge << " )" << " id: " << idStr;
    }
}

/*!
    \brief Проверяет, явлвяется ли данный элемент радиоактивным.

    Проводит поиск id элемента по списку dbIds (записи, хранимые в переменной, чтобы не обращаться к бд лишний раз)

 */
bool isotope::isIso(int mass, int charge)
{
    QString fId = QString::number(mass) + "0" + QString::number(charge) + "0";
    for(int i=0;i<dbIds.size();i++)
    {
        if(fId == dbIds[i])
        {
            return true;
        }
    }
    return false;
}

/*!
    \brief Моделирует радиоактивный распад изотопа за промежуток iterTime

    Исользует формулу школьной физики N = N0 * 2^(t/T) для распада

    Рассматривает три сценарция:
    1) осуществить только бета распад, не пересчитвая распределение продуктов распада
    2) осуществить только альфа распад, не пересчитвая распределение продуктов распада
    3) осуществить оба распада, расчитывая распределение продуктов распада
    (кажется, что не приступать сразу к 3 в среднем экономней, ведь таких изотопов не то чтобы много.
    При этом, если оставить только 3, програма по функциональности не потеряет ничего)

    P.S. вместо 2^(t/T) исользуется выражение e^(t/T*ln2), потому что операция поднесения bignumber не к целому числу не реализована, а я манал пока

 */
QPair < QVector<equalIsoData> , bignumber> isotope::doDecays(bignumber iterTime)
{
    QVector< equalIsoData > vc; // MC элемента(его информация) и кол-во. Такая формация полностью представляет изотоп
    // Что это за страшная структура данных?
    // Вектор состоит из таких пар: ( (маса,заряд) , кол-во )

    bignumber newQuantity = isoQuantity * ttmath::Exp(- iterTime/_halflife * M_LN2); // isoQuantity* 2 ^ (iterTime/_halflife) (2^x = e^(x*ln2)))
    bignumber decN = isoQuantity - newQuantity;

    // Далее присутствует развилка, которая призвана уменьшить время работы алгоритма:
    // Либо произвести расчеты лишь для альфа/бета распада, или для обеих, если необходимо(что вдвое медленнее)

    if(_beta_pr == 1) // Если совершается лишь бета-распад
    {
        isoQuantity = newQuantity;
        if(isIso(_mass,_charge+1)) // проверяем, является ли продукт распада изотопом
        {        
        vc.push_back(equalIsoData(QPair<int,int>(_mass,_charge+1),decN));
        }
    }
    else if(_alpha_pr == 1) // Если совершается лишь альфа-распад
    {
        isoQuantity = newQuantity;
        if(isIso(_mass-4,_charge-2)) // проверяем, является ли продукт распада изотопом
        {
        vc.push_back(equalIsoData(QPair<int,int>(_mass-4,_charge-2),decN));
        }
    }
    // также предусматриваем сценарий множественного распада (который выполняется вдвое медленней)
    else
    {
        isoQuantity = newQuantity;
        if(isIso(_mass-4,_charge-2)) // проверяем, является ли продукт альфа-распада изотопом
        {
        vc.push_back(equalIsoData(QPair<int,int>(_mass-4,_charge-2),decN*_alpha_pr));
        }
        if(isIso(_mass,_charge+1)) // проверяем, является ли продукт бета-распада изотопом
        {
        vc.push_back(equalIsoData(QPair<int,int>(_mass,_charge+1),decN*_beta_pr));
        }
    }
    return QPair < QVector<equalIsoData> , bignumber> (vc , decN); // return new isotope(s)
}

/*!
    \brief Функция, которая добавляет в изотоп новые элементы

    Функцию использует преимущественно radioactivemix. Так же ее может использовать интерфейс, для добавления в смесь изотопов элемента, который там уже есть

    Верхние элементы радиоактивного ряда переходят в нижние во время каждой итерации.
    Использование этой функции необходимо для целосности isotope_list в radioactivemix, чтобы после итерации в списке не хранилось несколько одинаковых элементов
 */
void isotope::addQuantity(bignumber supplement)
{
    isoQuantity += supplement;
}
