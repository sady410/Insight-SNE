#include "appwindowbuilder.h"
#include "explanationform.h"
#include "featuresform.h"
#include "overviewform.h"
#include "tsneform.h"
#include "jsonparser.h"
#include "appwindow.h"
#include <QHorizontalBarSeries>
#include <QBarSet>
#include <QScatterSeries>
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

    QList<qreal> meanDataList = appWindow->computeMeanData(dataList);
    QBarSeries *meanDataSeries = new QBarSeries();
    QBarSet *meanDataSet = new QBarSet("Mean features value");
    for (auto& value : meanDataList) {
        *meanDataSet << value;
    }
    meanDataSeries->append(meanDataSet);
    featuresForm->featuresPlot->initPlot(meanDataSeries, Qt::Vertical);

    return *this;
}

AppWindowBuilder& AppWindowBuilder::initializeExplanationPlots() {
    if (!appWindow) {
        appWindow = new AppWindow(parent);
    }
    ExplanationForm* explanationForm = appWindow->getExplanationForm();

    QJsonArray featureNames = jsonData.value("featureNames").toArray();
    QStringList featureNamesList;
    std::transform(featureNames.begin(), featureNames.end(), std::back_inserter(featureNamesList),
                   [](const QJsonValue &value) { return value.toString(); });
    explanationForm->magnitudesPlot->setLabels(featureNamesList);
    explanationForm->gradientsPlotTop->setLabels(featureNamesList);
    explanationForm->gradientsPlotBottom->setLabels(featureNamesList);

    QJsonArray meanGradients = jsonData.value("mean_gradients").toArray();
    QList<QList<qreal>> meanGradientsList = JsonParser::jsonArrayToQList2D(meanGradients);
    explanationForm->gradientsPlotTop->setMeanValues(meanGradientsList);
    explanationForm->gradientsPlotBottom->setMeanValues(meanGradientsList);

    QHorizontalBarSeries *meanGradientsXSeries = new QHorizontalBarSeries();
    QBarSet *meanGradientsXSet = new QBarSet("Mean Gradients X");
    for (auto& value : meanGradientsList[0]) {
        *meanGradientsXSet << value;
    }
    meanGradientsXSeries->append(meanGradientsXSet);
    explanationForm->gradientsPlotTop->initPlot(meanGradientsXSeries, Qt::Horizontal);

    QHorizontalBarSeries *meanGradientsYSeries = new QHorizontalBarSeries();
    QBarSet *meanGradientsYSet = new QBarSet("Mean Gradients Y");
    for (auto& value : meanGradientsList[1]) {
        *meanGradientsYSet << value;
    }
    meanGradientsYSeries->append(meanGradientsYSet);
    explanationForm->gradientsPlotBottom->initPlot(meanGradientsYSeries, Qt::Horizontal);

    QJsonArray gradients = jsonData.value("gradients").toArray();
    QList<QList<QList<qreal>>> gradientsList = JsonParser::jsonArrayToQList3D(gradients);
    explanationForm->gradientsPlotTop->setValues(appWindow->getAxisGradient(gradientsList, 0));
    explanationForm->gradientsPlotBottom->setValues(appWindow->getAxisGradient(gradientsList, 1));

    QJsonArray meanMagnitudes = jsonData.value("mean_magnitudes").toArray();
    QList<qreal> meanMagnitudesList = JsonParser::jsonArrayToQList(meanMagnitudes);
    QHorizontalBarSeries *meanMagnitudesSeries = new QHorizontalBarSeries();
    QBarSet *meanMagnitudesSet = new QBarSet("Mean Magnitudes");
    for (auto& value : meanMagnitudesList) {
        *meanMagnitudesSet << value;
    }
    meanMagnitudesSeries->append(meanMagnitudesSet);
    explanationForm->magnitudesPlot->initPlot(meanMagnitudesSeries, Qt::Horizontal);

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
    appWindow->show();
    return appWindow;
}
