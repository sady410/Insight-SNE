#include "appwindowbuilder.h"
#include "explanationform.h"
#include "featuresform.h"
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
    QJsonArray labels = m_jsonData.value("labels").toArray();
    QStringList labelsList;
    std::transform(labels.begin(), labels.end(), std::back_inserter(labelsList),
                   [](const QJsonValue &value) { return value.toString(); });
    tsneForm->tsnePlot->setLabels(labelsList);

    QJsonArray embedding = m_jsonData.value("Y").toArray();
    QScatterSeries *tsneSeries = new QScatterSeries();
    for (const QJsonValue &val : std::as_const(embedding)) {
        QJsonArray point = val.toArray();
        if (point.size() == 2) {
            qreal x = point[0].toDouble();
            qreal y = point[1].toDouble();
            tsneSeries->append(x, y);
        }
    }
    tsneSeries->setMarkerSize(8.0);

    tsneForm->tsnePlot->initPlot(tsneSeries);

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
    QScatterSeries *overviewSeries = new QScatterSeries();
    for (const QJsonValue &val : std::as_const(embedding)) {
        QJsonArray point = val.toArray();
        if (point.size() == 2) {
            qreal x = point[0].toDouble();
            qreal y = point[1].toDouble();
            overviewSeries->append(x, y);
        }
    }
    overviewSeries->setMarkerSize(8.0);
    m_appWindow->getOverviewForm()->overviewPlot->setScatterSeries(overviewSeries);

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

AppWindow* AppWindowBuilder::build() {
    if (!m_appWindow) {
        m_appWindow = new AppWindow(m_parent);
    }

    m_appWindow->setInitialized(true);
    m_appWindow->connectSignalsAndSlots();

    int i = 0;
    for (const QList<int>& indices : std::as_const(m_indicesList)) {
        if (i == 0) {
            m_appWindow->getSelectionManager()->createSelection(indices, false, true);
        } else {
            m_appWindow->getSelectionManager()->createSelection(indices, false, false);
        }
        i++;
    }

    m_appWindow->show();
    return m_appWindow;
}
