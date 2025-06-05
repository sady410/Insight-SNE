#include "appwindowbuilder.h"
#include "explanationform.h"
#include "featuresform.h"
#include "menuform.h"
#include "overviewform.h"
#include "tsneform.h"
#include "datamanager.h"
#include "appwindow.h"

// Constructor
AppWindowBuilder::AppWindowBuilder(QWidget* parent)
    : m_parent(parent), m_appWindow(nullptr)
{
    // Create shared data manager
    m_dataManager = DataManagerPtr::create();
}

// Set JSON data and parse it
AppWindowBuilder& AppWindowBuilder::setJsonData(const QJsonObject& jsonData) {
    if (!m_dataManager->loadJsonData(jsonData)) {
        qWarning() << "Failed to load JSON data in AppWindowBuilder";
    }
    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeTsnePlot() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    TsneForm* tsneForm = m_appWindow->getTsneForm();

    // Pass the data manager to the plot instead of copying data
    tsneForm->tsnePlot->setDataManager(m_dataManager);

    // Initialize plot with visualization type
    if (m_dataManager->isLabel()) {
        tsneForm->tsnePlot->initPlot(m_dataManager->isClass(), !m_dataManager->isClass());
    } else {
        tsneForm->tsnePlot->initPlot(false, false);
    }

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeOverviewPlot() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    OverviewForm* overviewForm = m_appWindow->getOverviewForm();

    // Pass data manager instead of copying scatter data
    overviewForm->overviewPlot->setDataManager(m_dataManager);

    QString infoText = QString("Dataset: %1 (%2 instances)\n"
                               "Perplexity: %3 | Iterations: %4")
                           .arg(m_dataManager->filename())
                           .arg(m_dataManager->dataSize())
                           .arg(m_dataManager->perplexity())
                           .arg(m_dataManager->iterations());
    overviewForm->setDatasetInfo(infoText);

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeFeaturesPlot() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    FeaturesForm* featuresForm = m_appWindow->getFeaturesForm();

    // Pass data manager instead of copying data
    featuresForm->featuresPlot->setDataManager(m_dataManager);
    featuresForm->featuresPlot->initPlot(Qt::Vertical, 3);

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeDistributionPlot() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    // FeaturesForm* featuresForm = m_appWindow->getFeaturesForm();

    // // Pass data manager instead of copying data
    // featuresForm->distributionPlot->setDataManager(m_dataManager);

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeExplanationPlots() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    ExplanationForm* explanationForm = m_appWindow->getExplanationForm();

    // Pass data manager to all explanation plots
    explanationForm->magnitudesPlot->setDataManager(m_dataManager);
    explanationForm->gradientsPlotTop->setDataManager(m_dataManager);
    explanationForm->gradientsPlotBottom->setDataManager(m_dataManager);

    // Initialize plots
    explanationForm->gradientsPlotTop->initPlot(Qt::Horizontal, 1);
    explanationForm->gradientsPlotBottom->initPlot(Qt::Horizontal, 2);
    explanationForm->magnitudesPlot->initPlot(Qt::Horizontal, 0);

    // The plots will now access gradients data directly from data manager
    // You might need to add methods to get axis-specific gradients

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeFeaturesForm() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    // FeaturesForm* featuresForm = m_appWindow->getFeaturesForm();

    // // Pass data manager instead of copying feature names
    // featuresForm->setDataManager(m_dataManager);

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeMenuForm() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    // MenuForm* menuForm = m_appWindow->getMenuForm();

    // // Pass data manager instead of copying feature names
    // menuForm->setDataManager(m_dataManager);

    // // Add filters to the menu using data manager
    // menuForm->addFilterToMenu(0, "Magnitudes", m_dataManager->featureNames());
    // menuForm->addFilterToMenu(1, "Attributes", m_dataManager->featureNames());

    // menuForm->addScopeToCombobox({"Global"});

    return *this;
}

AppWindow* AppWindowBuilder::build() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    // Pass data manager to app window
    // m_appWindow->setDataManager(m_dataManager);

    m_appWindow->setInitialized(true);
    m_appWindow->connectSignalsAndSlots();

    // Create selections using data from data manager
    const auto& indicesList = m_dataManager->indicesList();
    const auto& uniqueLabels = m_dataManager->uniqueLabels();

    for (int i = 0; i < indicesList.size(); ++i) {
        if (i == 0) {
            m_appWindow->getSelectionManager()->createSelection(indicesList[i], false, true, "All points");
        } else {
            if (i-1 < uniqueLabels.size() && m_dataManager->isClass()) {
                m_appWindow->getSelectionManager()->createSelection(indicesList[i], false, false, uniqueLabels[i-1]);
            }
        }
    }

    m_appWindow->getMenuForm()->connectModelSignals();
    m_appWindow->showMaximized();
    return m_appWindow;
}
