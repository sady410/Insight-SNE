#ifndef APPWINDOWBUILDER_H
#define APPWINDOWBUILDER_H

#include "appwindow.h"

#include <QScatterSeries>

class AppWindowBuilder {
public:
    AppWindowBuilder(QJsonObject json, QWidget *parent = nullptr)
        : m_jsonData(json), m_parent(parent) {}

    void extracted(QList<QList<int>> &indicesList,
                   QMap<QString, QList<int>> &classIndicesMap);
    AppWindowBuilder &initializeTsnePlot();
    AppWindowBuilder& initializeOverviewPlot();
    AppWindowBuilder& initializeFeaturesPlot();
    AppWindowBuilder& initializeExplanationPlots();
    AppWindowBuilder& initializeMenuForm();
    AppWindow* build();

private:
    QList<QList<int>> m_indicesList;
    QJsonObject m_jsonData;
    QWidget* m_parent;
    AppWindow* m_appWindow = nullptr;
};

#endif // APPWINDOWBUILDER_H
