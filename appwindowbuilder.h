#ifndef APPWINDOWBUILDER_H
#define APPWINDOWBUILDER_H

#include <QWidget>
#include <QJsonObject>
#include "datamanager.h"

class AppWindow;

class AppWindowBuilder
{
public:
    explicit AppWindowBuilder(QWidget* parent = nullptr);

    // Method to set JSON data
    AppWindowBuilder& setJsonData(const QJsonObject& jsonData);

    // Initialization methods
    AppWindowBuilder& initializeTsnePlot();
    AppWindowBuilder& initializeOverviewPlot();
    AppWindowBuilder& initializeFeaturesPlot();
    AppWindowBuilder& initializeDistributionPlot();
    AppWindowBuilder& initializeExplanationPlots();
    AppWindowBuilder& initializeFeaturesForm();
    AppWindowBuilder& initializeMenuForm();

    // Build the final app window
    AppWindow* build();

    // Getter for data manager (if needed elsewhere)
    DataManagerPtr dataManager() const { return m_dataManager; }

private:
    QWidget* m_parent;
    AppWindow* m_appWindow;
    DataManagerPtr m_dataManager;
};

#endif // APPWINDOWBUILDER_H
