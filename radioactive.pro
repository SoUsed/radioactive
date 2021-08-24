QT       += core gui charts sql testlib


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    isotope.cpp \
    main.cpp \
    radioactive.cpp \
    radioactivemix.cpp

HEADERS += \
    connectisotopedb.h \
    isotope.h \
    radioactive.h \
    radioactivemix.h \
    ttmath/ttmath.h \
    ttmath/ttmathbig.h \
    ttmath/ttmathdec.h \
    ttmath/ttmathint.h \
    ttmath/ttmathmisc.h \
    ttmath/ttmathobjects.h \
    ttmath/ttmathparser.h \
    ttmath/ttmaththreads.h \
    ttmath/ttmathtypes.h \
    ttmath/ttmathuint.h \
    ttmath/ttmathuint_noasm.h \
    ttmath/ttmathuint_x86.h \
    ttmath/ttmathuint_x86_64.h

FORMS += \
    radioactive.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    plotview.py \
    ttmath/ttmathuint_x86_64_msvc.asm \
    ttmath/win64_assemble.bat




