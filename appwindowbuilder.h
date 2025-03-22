#ifndef APPWINDOWBUILDER_H
#define APPWINDOWBUILDER_H

#include "appwindow.h"

#include <QScatterSeries>

class AppWindowBuilder {
public:
    AppWindowBuilder(QJsonObject json, QWidget *parent = nullptr)
        : jsonData(json), parent(parent) {}

    void extracted(QList<QList<int>> &indicesList,
                   QMap<QString, QList<int>> &classIndicesMap);
    AppWindowBuilder &initializeTsnePlot();
    AppWindowBuilder& initializeOverviewPlot();
    AppWindowBuilder& initializeFeaturesPlot();
    AppWindowBuilder& initializeExplanationPlots();
    AppWindow* build();

private:
    QList<QList<int>> indicesList;
    QJsonObject jsonData;
    QWidget* parent;
    AppWindow* appWindow = nullptr;
};

#endif // APPWINDOWBUILDER_H
