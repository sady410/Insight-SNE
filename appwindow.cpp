#include "appwindow.h"
#include "ui_appwindow.h"
#include "jsonparser.h"

#include <QScatterSeries>
#include <QHorizontalBarSeries>
#include <QBarSeries>
#include <QBarSet>
#include <QValueAxis>

AppWindow::AppWindow(QJsonObject json, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AppWindow)
    , jsonData(json)
{
    ui->setupUi(this);
    qDebug() << "AppWindow initialized with JSON data.";

    // Add feature names to plots that requires them
    QJsonArray featureNames = json.value("featureNames").toArray();
    QStringList featureNamesList;
    std::transform(featureNames.begin(), featureNames.end(), std::back_inserter(featureNamesList),
                   [](const QJsonValue &value) { return value.toString(); });
    ui->explanationForm->magnitudesPlot->setLabels(featureNamesList);
    ui->explanationForm->gradientsPlotTop->setLabels(featureNamesList);
    ui->explanationForm->gradientsPlotBottom->setLabels(featureNamesList);
    ui->featuresForm->featuresPlot->setLabels(featureNamesList);

    // Tsne Plot
    QJsonArray labels = json.value("labels").toArray();
    QStringList labelsList;
    std::transform(labels.begin(), labels.end(), std::back_inserter(labelsList),
                   [](const QJsonValue &value) { return value.toString(); });
    ui->tsneForm->tsnePlot->setLabels(labelsList);

    QJsonArray embedding = json.value("Y").toArray();
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
    ui->tsneForm->tsnePlot->initPlot(tsneSeries);

    // Overview Plot
    QScatterSeries *overviewSeries = new QScatterSeries();
    const auto &points = tsneSeries->points();  // Store reference to avoid detachment
    for (const QPointF &point : points) {
        overviewSeries->append(point);
    }
    overviewSeries->setMarkerSize(tsneSeries->markerSize());
    // overviewSeries->setColor(tsneSeries->color());  // To keep the same style

    ui->overviewForm->overviewPlot->setScatterSeries(overviewSeries);

    // Features Plot
    QJsonArray data = json.value("data").toArray();
    QList<QList<qreal>> dataList = JsonParser::jsonArrayToQList2D(data);
    ui->featuresForm->featuresPlot->setValues(dataList);

    QList<qreal> meanDataList = computeMeanData(dataList);
    QBarSeries *meanDataSeries = new QBarSeries();
    QBarSet *meanDataSet = new QBarSet("Mean features value");
    for (auto& value : meanDataList) {
        *meanDataSet << value;
    }
    meanDataSeries->append(meanDataSet);
    ui->featuresForm->featuresPlot->initPlot(meanDataSeries, BarPlot::Vertical);

    // Explanation Plots
    QJsonArray meanGradients = json.value("mean_gradients").toArray();
    QList<QList<qreal>> meanGradientsList = JsonParser::jsonArrayToQList2D(meanGradients);
    ui->explanationForm->gradientsPlotTop->setMeanValues(meanGradientsList);
    ui->explanationForm->gradientsPlotBottom->setMeanValues(meanGradientsList);

    QHorizontalBarSeries *meanGradientsXSeries = new QHorizontalBarSeries();
    QBarSet *meanGradientsXSet = new QBarSet("Mean Gradients X");
    for (auto& value : meanGradientsList[0]) {
        *meanGradientsXSet << value;
    }
    meanGradientsXSeries->append(meanGradientsXSet);
    ui->explanationForm->gradientsPlotTop->initPlot(meanGradientsXSeries, BarPlot::Horizontal);

    QHorizontalBarSeries *meanGradientsYSeries = new QHorizontalBarSeries();
    QBarSet *meanGradientsYSet = new QBarSet("Mean Gradients Y");
    for (auto& value : meanGradientsList[1]) {
        *meanGradientsYSet << value;
    }
    meanGradientsYSeries->append(meanGradientsYSet);
    ui->explanationForm->gradientsPlotBottom->initPlot(meanGradientsYSeries, BarPlot::Horizontal);

    QJsonArray gradients = json.value("gradients").toArray();
    QList<QList<QList<qreal>>> gradientsList = JsonParser::jsonArrayToQList3D(gradients);
    ui->explanationForm->gradientsPlotTop->setValues(getAxisGradient(gradientsList, 0));
    ui->explanationForm->gradientsPlotBottom->setValues(getAxisGradient(gradientsList, 1));

    QJsonArray meanMagnitudes = json.value("mean_magnitudes").toArray();
    QList<qreal> meanMagnitudesList = JsonParser::jsonArrayToQList(meanMagnitudes);
    QHorizontalBarSeries *meanMagnitudesSeries = new QHorizontalBarSeries();
    QBarSet *meanMagnitudesSet = new QBarSet("Mean Magnitudes");
    for (auto& value : meanMagnitudesList) {
        *meanMagnitudesSet << value;
    }
    meanMagnitudesSeries->append(meanMagnitudesSet);
    ui->explanationForm->magnitudesPlot->initPlot(meanMagnitudesSeries, BarPlot::Horizontal);

    QJsonArray magnitudes = json.value("magnitudes").toArray();
    QList<QList<qreal>> magnitudesList = JsonParser::jsonArrayToQList2D(magnitudes);
    ui->explanationForm->magnitudesPlot->setValues(magnitudesList);

    // Connect signals and slots
    connect(ui->tsneForm->tsnePlot, &TsnePlot::pointsSelected, ui->explanationForm->magnitudesPlot, &BarPlot::addValue);
    connect(ui->tsneForm->tsnePlot, &TsnePlot::pointsSelected, ui->explanationForm->gradientsPlotTop, &BarPlot::addValue);
    connect(ui->tsneForm->tsnePlot, &TsnePlot::pointsSelected, ui->explanationForm->gradientsPlotBottom, &BarPlot::addValue);
    connect(ui->tsneForm->tsnePlot, &TsnePlot::pointsSelected, ui->featuresForm->featuresPlot, &BarPlot::addValue);

    connect(ui->tsneForm->tsnePlot, &TsnePlot::zoomChanged, ui->overviewForm->overviewPlot, &OverviewPlot::updateZoomRectangle);

    isInitialized = true; // Avoid resize event issue
    show();
}

QList<qreal> AppWindow::computeMeanData(const QList<QList<qreal>> &dataList) {
    QList<qreal> meanDataList;

    if (dataList.isEmpty()) {
        return meanDataList; // Return empty list if input is empty
    }

    // Determine the number of columns (assuming all rows have the same number of columns)
    int numColumns = dataList[0].size();

    // Initialize the meanDataList with zeros
    meanDataList.resize(numColumns);
    std::fill(meanDataList.begin(), meanDataList.end(), 0.0);

    // Iterate through each row and accumulate column values
    for (const QList<qreal> &row : dataList) {
        if (row.size() != numColumns) {
            qWarning() << "Row size mismatch! Skipping row.";
            continue;
        }

        for (int col = 0; col < numColumns; ++col) {
            meanDataList[col] += row[col];
        }
    }

    // Divide by the number of rows to compute the mean for each column
    int numRows = dataList.size();
    for (int col = 0; col < numColumns; ++col) {
        meanDataList[col] /= numRows;
    }

    return meanDataList;
}

QList<QList<qreal>> AppWindow::getAxisGradient(const QList<QList<QList<qreal>>> &gradientsList, int axis) {
    QList<QList<qreal>> result;

    for (const QList<QList<qreal>> &gradient : gradientsList) {
        if (axis >= 0 && axis < gradient.size()) {
            result.append(gradient[axis]);
        } else {
            qWarning() << "Axis out of bounds!";
            return QList<QList<qreal>>();
        }
    }

    return result;
}

void AppWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    if (!isInitialized) {
        return;
    }
    // Force OverviewPlot to update zoom rectangle
    QValueAxis *axisX = qobject_cast<QValueAxis *>(ui->tsneForm->tsnePlot->chart()->axes(Qt::Horizontal).first());
    QValueAxis *axisY = qobject_cast<QValueAxis *>(ui->tsneForm->tsnePlot->chart()->axes(Qt::Vertical).first());

    if (axisX && axisY) {
        emit ui->tsneForm->tsnePlot->zoomChanged(axisX->min(), axisX->max(), axisY->min(), axisY->max());
    }
}

AppWindow::~AppWindow()
{
    delete ui;
}
