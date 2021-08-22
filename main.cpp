#include "radioactive.h"

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    radioactive mainWind;
    mainWind.setGeometry(app.screens()[0]->geometry());
    mainWind.show();
    return app.exec();
}
