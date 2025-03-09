#ifndef APPWINDOWBUILDER_H
#define APPWINDOWBUILDER_H

#include "appwindow.h"

class AppWindowBuilder {
public:
    AppWindowBuilder(QJsonObject json, QWidget *parent = nullptr)
        : jsonData(json), parent(parent) {}

    AppWindowBuilder& initializeTsnePlot();
    AppWindowBuilder& initializeOverviewPlot();
    AppWindowBuilder& initializeFeaturesPlot();
    AppWindowBuilder& initializeExplanationPlots();
    AppWindow* build();

private:
    QJsonObject jsonData;
    QWidget* parent;
    AppWindow* appWindow = nullptr;
};

#endif // APPWINDOWBUILDER_H
