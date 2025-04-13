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

AppWindowBuilder& AppWindowBuilder::initializeTsnePlot() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }
    TsneForm* tsneForm = m_appWindow->getTsneForm();

    // Get labels
    QJsonArray labels = m_jsonData.value("labels").toArray();
    QStringList labelsList;
    std::transform(labels.begin(), labels.end(), std::back_inserter(labelsList),
                   [](const QJsonValue &value) { return value.toString(); });
    tsneForm->tsnePlot->setLabels(labelsList);

    // Get feature names
    QJsonArray featureNames = m_jsonData.value("featureNames").toArray();
    QStringList featureNamesList;
    std::transform(featureNames.begin(), featureNames.end(), std::back_inserter(featureNamesList),
                   [](const QJsonValue &value) { return value.toString(); });
    tsneForm->tsnePlot->setFeatureNames(featureNamesList);

    // Get gradients
    QJsonArray gradients = m_jsonData.value("gradients").toArray();
    QList<QList<QList<qreal>>> gradientsList = JsonParser::jsonArrayToQList3D(gradients);
    tsneForm->tsnePlot->setGradients(gradientsList);

    // Get ZValues
    QJsonArray zValues = m_jsonData.value("data").toArray();
    QList<QList<qreal>> zValuesList = JsonParser::jsonArrayToQList2D(zValues);
    tsneForm->tsnePlot->setZValues(zValuesList);

    // Get embedding data and convert to QVector<double>
    QJsonArray embedding = m_jsonData.value("Y").toArray();
    QVector<double> xData, yData;
    for (const QJsonValue &val : std::as_const(embedding)) {
        QJsonArray point = val.toArray();
        if (point.size() == 2) {
            xData.append(point[0].toDouble());
            yData.append(point[1].toDouble());
        }
    }

    // Initialize plot with the vectors directly
    tsneForm->tsnePlot->initPlot(xData, yData);

    // Process labels and indices
    m_uniqueLabels = labelsList;
    m_uniqueLabels.removeDuplicates();

    // Create the list of lists (indices)
    QMap<QString, QList<int>> classIndicesMap;
    QList<int> allIndices;

    for (int i = 0; i < labelsList.size(); ++i) {
        allIndices.append(i);
        classIndicesMap[labelsList[i]].append(i);
    }

    m_indicesList.append(allIndices);
    for (const auto& indices : std::as_const(classIndicesMap)) {
        m_indicesList.append(indices);
    }

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeOverviewPlot() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    QJsonArray embedding = m_jsonData.value("Y").toArray();
    QVector<double> xData, yData;

    // Reserve space for better performance with large datasets
    xData.reserve(embedding.size());
    yData.reserve(embedding.size());

    // Extract data from JSON
    for (const QJsonValue &val : std::as_const(embedding)) {
        QJsonArray point = val.toArray();
        if (point.size() == 2) {
            xData.append(point[0].toDouble());
            yData.append(point[1].toDouble());
        }
    }

    // Set the data to the overview plot
    m_appWindow->getOverviewForm()->overviewPlot->setScatterData(xData, yData);

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeFeaturesPlot() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }
    FeaturesForm* featuresForm = m_appWindow->getFeaturesForm();

    QJsonArray featureNames = m_jsonData.value("featureNames").toArray();
    QStringList featureNamesList;
    std::transform(featureNames.begin(), featureNames.end(), std::back_inserter(featureNamesList),
                   [](const QJsonValue &value) { return value.toString(); });
    featuresForm->featuresPlot->setLabels(featureNamesList);

    QJsonArray data = m_jsonData.value("data").toArray();
    QList<QList<qreal>> dataList = JsonParser::jsonArrayToQList2D(data);
    featuresForm->featuresPlot->setValues(dataList);
    featuresForm->featuresPlot->initPlot(Qt::Vertical);

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeExplanationPlots() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }
    ExplanationForm* explanationForm = m_appWindow->getExplanationForm();

    // Set labels
    QJsonArray featureNames = m_jsonData.value("featureNames").toArray();
    QStringList featureNamesList;
    std::transform(featureNames.begin(), featureNames.end(), std::back_inserter(featureNamesList),
                   [](const QJsonValue &value) { return value.toString(); });
    explanationForm->magnitudesPlot->setLabels(featureNamesList);
    explanationForm->gradientsPlotTop->setLabels(featureNamesList);
    explanationForm->gradientsPlotBottom->setLabels(featureNamesList);

    // Gradients plot
    explanationForm->gradientsPlotTop->initPlot(Qt::Horizontal);
    explanationForm->gradientsPlotBottom->initPlot(Qt::Horizontal);

    QJsonArray gradients = m_jsonData.value("gradients").toArray();
    QList<QList<QList<qreal>>> gradientsList = JsonParser::jsonArrayToQList3D(gradients);
    explanationForm->gradientsPlotTop->setValues(m_appWindow->getAxisGradient(gradientsList, 0));
    explanationForm->gradientsPlotBottom->setValues(m_appWindow->getAxisGradient(gradientsList, 1));

    // Magnitudes plot
    explanationForm->magnitudesPlot->initPlot(Qt::Horizontal);

    QJsonArray magnitudes = m_jsonData.value("magnitudes").toArray();
    QList<QList<qreal>> magnitudesList = JsonParser::jsonArrayToQList2D(magnitudes);
    explanationForm->magnitudesPlot->setValues(magnitudesList);

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeMenuForm() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    QJsonArray featureNames = m_jsonData.value("featureNames").toArray();
    QStringList featureNamesList;
    std::transform(featureNames.begin(), featureNames.end(), std::back_inserter(featureNamesList),
                   [](const QJsonValue &value) { return value.toString(); });

    MenuForm* menuForm = m_appWindow->getMenuForm();

    menuForm->setFeatureNames(featureNamesList);

    return *this;
}

AppWindow* AppWindowBuilder::build() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    m_appWindow->setInitialized(true);
    m_appWindow->connectSignalsAndSlots();
    qDebug() << m_uniqueLabels;
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
