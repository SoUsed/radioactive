#ifndef RADIOACTIVE_H
#define RADIOACTIVE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class radioactive; }
QT_END_NAMESPACE

class radioactive : public QWidget
{
    Q_OBJECT

public:
    radioactive(QWidget *parent = nullptr);
    ~radioactive();

private:
    Ui::radioactive *ui;
};
#endif // RADIOACTIVE_H
