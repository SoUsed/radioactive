#include "radioactive.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    radioactive w;
    w.show();
    return a.exec();
}
