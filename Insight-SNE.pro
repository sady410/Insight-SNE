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

INCLUDEPATH += \
    headers \
    headers/windows \
    headers/forms \
    headers/plots \
    headers/libs \
    headers/utils \
    headers/managers \
    headers/widgets \

SOURCES += \
    sources/windows/appwindow.cpp \
    sources/windows/appwindowbuilder.cpp \
    sources/managers/arrowsmanager.cpp \
    sources/plots/barplot.cpp \
    sources/widgets/colorcircle.cpp \
    sources/forms/comparisonform.cpp \
    sources/managers/datamanager.cpp \
    sources/plots/distributionplot.cpp \
    sources/forms/explanationform.cpp \
    sources/forms/featuresform.cpp \
    sources/utils/jsonparser.cpp \
    sources/main.cpp \
    sources/forms/menuform.cpp \
    sources/windows/menuwindow.cpp \
    sources/forms/overviewform.cpp \
    sources/plots/overviewplot.cpp \
    sources/utils/pythonexecutor.cpp \
    sources/utils/scopecomputation.cpp \
    sources/managers/selectionmanager.cpp \
    sources/widgets/selectionwidget.cpp \
    sources/utils/thememanager.cpp \
    sources/forms/tsneform.cpp \
    sources/plots/tsneplot.cpp \
    sources/libs/qcustomplot.cpp

HEADERS += \
    headers/windows/appwindow.h \
    headers/windows/appwindowbuilder.h \
    headers/managers/arrowsmanager.h \
    headers/plots/barplot.h \
    headers/widgets/colorcircle.h \
    headers/forms/comparisonform.h \
    headers/managers/datamanager.h \
    headers/plots/distributionplot.h \
    headers/forms/explanationform.h \
    headers/forms/featuresform.h \
    headers/utils/jsonparser.h \
    headers/forms/menuform.h \
    headers/windows/menuwindow.h \
    headers/forms/overviewform.h \
    headers/plots/overviewplot.h \
    headers/utils/pythonexecutor.h \
    headers/utils/scopecomputation.h \
    headers/managers/selectionmanager.h \
    headers/widgets/selectionwidget.h \
    headers/utils/thememanager.h \
    headers/forms/tsneform.h \
    headers/plots/tsneplot.h \
    headers/libs/qcustomplot.h

FORMS += \
    ui/appwindow.ui \
    ui/comparisonform.ui \
    ui/explanationform.ui \
    ui/featuresform.ui \
    ui/menuform.ui \
    ui/menuwindow.ui \
    ui/overviewform.ui \
    ui/tsneform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    scripts/gradients.py \
    scripts/main.py \
    scripts/tsne.py

RESOURCES += \
    ressources/resources.qrc
