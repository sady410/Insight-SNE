QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets charts

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    appwindow.cpp \
    explanationform.cpp \
    explanationplot.cpp \
    main.cpp \
    menuwindow.cpp \
    overviewform.cpp \
    pythonexecutor.cpp \
    tsneform.cpp \
    tsneplot.cpp

HEADERS += \
    appwindow.h \
    explanationform.h \
    explanationplot.h \
    menuwindow.h \
    overviewform.h \
    pythonexecutor.h \
    tsneform.h \
    tsneplot.h

FORMS += \
    appwindow.ui \
    explanationform.ui \
    menuwindow.ui \
    overviewform.ui \
    tsneform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    gradients.py \
    main.py \
    tsne.py
