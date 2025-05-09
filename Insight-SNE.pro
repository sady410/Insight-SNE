QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets charts printsupport

CONFIG += c++17

unix:!android {
    # QCustomPlot configuration
    # Link against the library
    debug {
        LIBS += -L/usr/local/lib -lqcustomplotd
    } else {
        LIBS += -L/usr/local/lib -lqcustomplot
    }

    # Include path (not strictly needed since it's in /usr/local/include)
    INCLUDEPATH += /usr/local/include

    # You can make your code fail to compile if it uses deprecated APIs.
    # In order to do so, uncomment the following line.
    # DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
}

SOURCES += \
    appwindow.cpp \
    appwindowbuilder.cpp \
    arrowsmanager.cpp \
    barplot.cpp \
    colorcircle.cpp \
    comparisonform.cpp \
    distributionplot.cpp \
    explanationform.cpp \
    featuresform.cpp \
    jsonparser.cpp \
    main.cpp \
    menuform.cpp \
    menuwindow.cpp \
    overviewform.cpp \
    overviewplot.cpp \
    pythonexecutor.cpp \
    selectionmanager.cpp \
    selectionwidget.cpp \
    thememanager.cpp \
    tsneform.cpp \
    tsneplot.cpp \
    qcustomplot.cpp

HEADERS += \
    appwindow.h \
    appwindowbuilder.h \
    arrowsmanager.h \
    barplot.h \
    colorcircle.h \
    comparisonform.h \
    distributionplot.h \
    explanationform.h \
    featuresform.h \
    jsonparser.h \
    menuform.h \
    menuwindow.h \
    overviewform.h \
    overviewplot.h \
    pythonexecutor.h \
    selectionmanager.h \
    selectionwidget.h \
    thememanager.h \
    tsneform.h \
    tsneplot.h \
    qcustomplot.h

FORMS += \
    appwindow.ui \
    comparisonform.ui \
    explanationform.ui \
    featuresform.ui \
    menuform.ui \
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

RESOURCES += \
    resources.qrc
