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
    if (!appWindow) {
        appWindow = new AppWindow(parent);
    }
    TsneForm* tsneForm = appWindow->getTsneForm();
    QJsonArray labels = jsonData.value("labels").toArray();
    QStringList labelsList;
    std::transform(labels.begin(), labels.end(), std::back_inserter(labelsList),
                   [](const QJsonValue &value) { return value.toString(); });
    tsneForm->tsnePlot->setLabels(labelsList);

    QJsonArray embedding = jsonData.value("Y").toArray();
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

    indicesList.append(allIndices);
    for (const auto& indices : std::as_const(classIndicesMap)) {
        indicesList.append(indices);
    }

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeOverviewPlot() {
    if (!appWindow) {
        appWindow = new AppWindow(parent);
    }

    QJsonArray embedding = jsonData.value("Y").toArray();
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
    appWindow->getOverviewForm()->overviewPlot->setScatterSeries(overviewSeries);

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeFeaturesPlot() {
    if (!appWindow) {
        appWindow = new AppWindow(parent);
    }
    FeaturesForm* featuresForm = appWindow->getFeaturesForm();

    QJsonArray featureNames = jsonData.value("featureNames").toArray();
    QStringList featureNamesList;
    std::transform(featureNames.begin(), featureNames.end(), std::back_inserter(featureNamesList),
                   [](const QJsonValue &value) { return value.toString(); });
    featuresForm->featuresPlot->setLabels(featureNamesList);

    QJsonArray data = jsonData.value("data").toArray();
    QList<QList<qreal>> dataList = JsonParser::jsonArrayToQList2D(data);
    featuresForm->featuresPlot->setValues(dataList);
    featuresForm->featuresPlot->initPlot(Qt::Vertical);

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeExplanationPlots() {
    if (!appWindow) {
        appWindow = new AppWindow(parent);
    }
    ExplanationForm* explanationForm = appWindow->getExplanationForm();

    // Set labels
    QJsonArray featureNames = jsonData.value("featureNames").toArray();
    QStringList featureNamesList;
    std::transform(featureNames.begin(), featureNames.end(), std::back_inserter(featureNamesList),
                   [](const QJsonValue &value) { return value.toString(); });
    explanationForm->magnitudesPlot->setLabels(featureNamesList);
    explanationForm->gradientsPlotTop->setLabels(featureNamesList);
    explanationForm->gradientsPlotBottom->setLabels(featureNamesList);

    // Gradients plot
    explanationForm->gradientsPlotTop->initPlot(Qt::Horizontal);
    explanationForm->gradientsPlotBottom->initPlot(Qt::Horizontal);

    QJsonArray gradients = jsonData.value("gradients").toArray();
    QList<QList<QList<qreal>>> gradientsList = JsonParser::jsonArrayToQList3D(gradients);
    explanationForm->gradientsPlotTop->setValues(appWindow->getAxisGradient(gradientsList, 0));
    explanationForm->gradientsPlotBottom->setValues(appWindow->getAxisGradient(gradientsList, 1));

    // Magnitudes plot
    explanationForm->magnitudesPlot->initPlot(Qt::Horizontal);

    QJsonArray magnitudes = jsonData.value("magnitudes").toArray();
    QList<QList<qreal>> magnitudesList = JsonParser::jsonArrayToQList2D(magnitudes);
    explanationForm->magnitudesPlot->setValues(magnitudesList);

    return *this;
}

AppWindow* AppWindowBuilder::build() {
    if (!appWindow) {
        appWindow = new AppWindow(parent);
    }

    appWindow->setInitialized(true);
    appWindow->connectSignalsAndSlots();
    appWindow->getSelectionManager()->createMultipleSelections(indicesList);
    appWindow->show();
    return appWindow;
}
