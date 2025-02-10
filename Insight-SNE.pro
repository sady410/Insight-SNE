QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets charts

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    appwindow.cpp \
    # explanationplot.cpp \
    main.cpp \
    menuwindow.cpp \
    pythonexecutor.cpp \
    tsneplot.cpp

HEADERS += \
    appwindow.h \
    # explanationplot.h \
    menuwindow.h \
    pythonexecutor.h \
    tsneplot.h

FORMS += \
    appwindow.ui \
    menuwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    test.py
