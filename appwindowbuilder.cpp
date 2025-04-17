#include "appwindowbuilder.h"
#include "explanationform.h"
#include "featuresform.h"
#include "menuform.h"
#include "overviewform.h"
#include "tsneform.h"
#include "jsonparser.h"
#include "appwindow.h"

#include <QHorizontalBarSeries>
#include <QBarSet>
#include <QBarSeries>

// Constructor - initialize shared data
AppWindowBuilder::AppWindowBuilder(QWidget* parent) : m_parent(parent), m_appWindow(nullptr) {
    // Initialize any other members
}

// Parse all JSON data once - new method
AppWindowBuilder& AppWindowBuilder::parseJsonData() {
    if (m_jsonData.isEmpty()) {
        return *this; // No data to parse
    }

    // Parse common arrays once
    m_embeddingArray = m_jsonData.value("Y").toArray();
    m_labelsArray = m_jsonData.value("labels").toArray();
    m_featureNamesArray = m_jsonData.value("featureNames").toArray();
    m_gradientsArray = m_jsonData.value("gradients").toArray();
    m_magnitudesArray = m_jsonData.value("magnitudes").toArray();
    m_attributesArray = m_jsonData.value("data").toArray();

    // Convert to appropriate data structures
    // Feature names used by multiple plots
    m_featureNamesList.clear();
    m_featureNamesList.reserve(m_featureNamesArray.size());
    for (const QJsonValue &value : std::as_const(m_featureNamesArray)) {
        m_featureNamesList.append(value.toString());
    }

    // Labels only used by tsne plot, but processed here for clarity
    m_labelsList.clear();
    m_labelsList.reserve(m_labelsArray.size());
    for (const QJsonValue &value : std::as_const(m_labelsArray)) {
        m_labelsList.append(value.toString());
    }

    // Extract embedding coordinates
    m_xData.clear();
    m_yData.clear();
    m_xData.reserve(m_embeddingArray.size());
    m_yData.reserve(m_embeddingArray.size());

    for (const QJsonValue &val : std::as_const(m_embeddingArray)) {
        QJsonArray point = val.toArray();
        if (point.size() == 2) {
            m_xData.append(point[0].toDouble());
            m_yData.append(point[1].toDouble());
        }
    }

    // Process gradients and magnitudes lazily - only convert when needed

    return *this;
}

// Lazily convert JSON to data structures
const QList<QList<QList<qreal>>>& AppWindowBuilder::getGradientsList() {
    if (m_gradientsList.isEmpty() && !m_gradientsArray.isEmpty()) {
        m_gradientsList = JsonParser::jsonArrayToQList3D(m_gradientsArray);
    }
    return m_gradientsList;
}

const QList<QList<qreal>>& AppWindowBuilder::getMagnitudesList() {
    if (m_magnitudesList.isEmpty() && !m_magnitudesArray.isEmpty()) {
        m_magnitudesList = JsonParser::jsonArrayToQList2D(m_magnitudesArray);
    }
    return m_magnitudesList;
}

const QList<QList<qreal>>& AppWindowBuilder::getAttributesList() {
    if (m_attributesList.isEmpty() && !m_attributesArray.isEmpty()) {
        m_attributesList = JsonParser::jsonArrayToQList2D(m_attributesArray);
    }
    return m_attributesList;
}

AppWindowBuilder& AppWindowBuilder::initializeTsnePlot() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }
    TsneForm* tsneForm = m_appWindow->getTsneForm();

    // Use already parsed data
    tsneForm->tsnePlot->setLabels(m_labelsList);
    tsneForm->tsnePlot->setFeatureNames(m_featureNamesList);

    // Use references to shared data
    tsneForm->tsnePlot->setGradients(getGradientsList());
    tsneForm->tsnePlot->setMagnitudes(getMagnitudesList());
    tsneForm->tsnePlot->setAttributes(getAttributesList());

    // Initialize plot with the vectors directly
    tsneForm->tsnePlot->initPlot(m_xData, m_yData);

    // Process labels and indices if not already done
    if (m_indicesList.isEmpty()) {
        // Create unique labels
        QSet<QString> uniqueLabelsSet(m_labelsList.begin(), m_labelsList.end());
        m_uniqueLabels = uniqueLabelsSet.values();

        // Create indices map
        QMap<QString, QList<int>> classIndicesMap;
        QList<int> allIndices;
        allIndices.reserve(m_labelsList.size());

        for (int i = 0; i < m_labelsList.size(); ++i) {
            allIndices.append(i);
            classIndicesMap[m_labelsList[i]].append(i);
        }

        m_indicesList.append(allIndices);
        for (const auto& label : m_uniqueLabels) {
            m_indicesList.append(classIndicesMap[label]);
        }
    }

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeOverviewPlot() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    // Use already parsed embedding data
    m_appWindow->getOverviewForm()->overviewPlot->setScatterData(m_xData, m_yData);

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeFeaturesPlot() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }
    FeaturesForm* featuresForm = m_appWindow->getFeaturesForm();

    // Use already parsed feature names
    featuresForm->featuresPlot->setLabels(m_featureNamesList);

    // Use shared data
    featuresForm->featuresPlot->setValues(getAttributesList());
    featuresForm->featuresPlot->initPlot(Qt::Vertical);

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeExplanationPlots() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }
    ExplanationForm* explanationForm = m_appWindow->getExplanationForm();

    // Set labels using already parsed feature names
    explanationForm->magnitudesPlot->setLabels(m_featureNamesList);
    explanationForm->gradientsPlotTop->setLabels(m_featureNamesList);
    explanationForm->gradientsPlotBottom->setLabels(m_featureNamesList);

    // Initialize plots
    explanationForm->gradientsPlotTop->initPlot(Qt::Horizontal);
    explanationForm->gradientsPlotBottom->initPlot(Qt::Horizontal);
    explanationForm->magnitudesPlot->initPlot(Qt::Horizontal);

    // Use shared data
    const QList<QList<QList<qreal>>>& gradientsList = getGradientsList();
    explanationForm->gradientsPlotTop->setValues(m_appWindow->getAxisGradient(gradientsList, 0));
    explanationForm->gradientsPlotBottom->setValues(m_appWindow->getAxisGradient(gradientsList, 1));

    // Set magnitudes
    explanationForm->magnitudesPlot->setValues(getMagnitudesList());

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeMenuForm() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    MenuForm* menuForm = m_appWindow->getMenuForm();

    // Use already parsed feature names
    menuForm->setFeatureNames(m_featureNamesList);

    // Add filters to the menu
    menuForm->addFilterToMenu(0, "Magnitudes", m_featureNamesList);
    menuForm->addFilterToMenu(1, "Attributes", m_featureNamesList);
    menuForm->addFilterToMenu(2, "Scope", {"TODO"});

    return *this;
}

AppWindow* AppWindowBuilder::build() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    m_appWindow->setInitialized(true);
    m_appWindow->connectSignalsAndSlots();

    int i = 0;
    for (const QList<int>& indices : std::as_const(m_indicesList)) {
        if (i == 0) {
            m_appWindow->getSelectionManager()->createSelection(indices, false, true, "All points");
        } else {
            if (i-1 < m_uniqueLabels.size()) {
                m_appWindow->getSelectionManager()->createSelection(indices, false, false, m_uniqueLabels[i-1]);
            }
        }
        i++;
    }

    m_appWindow->show();
    return m_appWindow;
}

// New method to set JSON data and parse it in one step
AppWindowBuilder& AppWindowBuilder::setJsonData(const QJsonObject& jsonData) {
    m_jsonData = jsonData;
    return parseJsonData();
}
