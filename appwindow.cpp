#include "appwindow.h"
#include "qjsonobject.h"
#include "ui_appwindow.h"

#include <QJsonArray>
#include <QScatterSeries>
#include <QHorizontalBarSeries>
#include <QBarSet>
#include <QValueAxis>

AppWindow::AppWindow(QJsonObject json, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AppWindow)
    , jsonData(json)
{
    ui->setupUi(this);
    qDebug() << "AppWindow initialized with JSON data.";

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

    // Explanation Plots
    QJsonArray featureNames = json.value("featureNames").toArray();
    QStringList featureNamesList;
    std::transform(featureNames.begin(), featureNames.end(), std::back_inserter(featureNamesList),
                   [](const QJsonValue &value) { return value.toString(); });
    ui->explanationForm->magnitudesPlot->setFeatureNames(featureNamesList);
    ui->explanationForm->gradientsPlotTop->setFeatureNames(featureNamesList);
    ui->explanationForm->gradientsPlotBottom->setFeatureNames(featureNamesList);

    QJsonArray meanGradients = json.value("mean_gradients").toArray();
    QList<QList<qreal>> meanGradientsList = jsonArrayToQList2D(meanGradients);
    ui->explanationForm->gradientsPlotTop->setMeanGradients(meanGradientsList);
    ui->explanationForm->gradientsPlotBottom->setMeanGradients(meanGradientsList);

    QHorizontalBarSeries *meanGradientsXSeries = new QHorizontalBarSeries();
    QBarSet *meanGradientsXSet = new QBarSet("Mean Gradients X");
    for (auto& value : meanGradientsList[0]) {
        *meanGradientsXSet << value;
    }
    meanGradientsXSeries->append(meanGradientsXSet);
    ui->explanationForm->gradientsPlotTop->initPlot(meanGradientsXSeries);

    QHorizontalBarSeries *meanGradientsYSeries = new QHorizontalBarSeries();
    QBarSet *meanGradientsYSet = new QBarSet("Mean Gradients Y");
    for (auto& value : meanGradientsList[1]) {
        *meanGradientsYSet << value;
    }
    meanGradientsYSeries->append(meanGradientsYSet);
    ui->explanationForm->gradientsPlotBottom->initPlot(meanGradientsYSeries);

    QJsonArray gradients = json.value("gradients").toArray();
    QList<QList<QList<qreal>>> gradientsList = jsonArrayToQList3D(gradients);
    ui->explanationForm->gradientsPlotTop->setGradients(getAxisGradient(gradientsList, 0));
    ui->explanationForm->gradientsPlotBottom->setGradients(getAxisGradient(gradientsList, 1));

    QJsonArray meanMagnitudes = json.value("mean_magnitudes").toArray();
    QList<qreal> meanMagnitudesList = jsonArrayToQList(meanMagnitudes);
    QHorizontalBarSeries *series = new QHorizontalBarSeries();
    QBarSet *set = new QBarSet("Mean Magnitudes");
    for (auto& value : meanMagnitudesList) {
        *set << value;
    }
    series->append(set);
    ui->explanationForm->magnitudesPlot->initPlot(series);

    QJsonArray magnitudes = json.value("magnitudes").toArray();
    QList<QList<qreal>> magnitudesList = jsonArrayToQList2D(magnitudes);
    ui->explanationForm->magnitudesPlot->setMagnitudes(magnitudesList);

    connect(ui->tsneForm->tsnePlot, &TsnePlot::pointsSelected, ui->explanationForm->magnitudesPlot, &MagnitudesPlot::updateSelectedPoints);
    connect(ui->tsneForm->tsnePlot, &TsnePlot::pointsSelected, ui->explanationForm->gradientsPlotTop, &GradientsPlot::updateSelectedPoints);
    connect(ui->tsneForm->tsnePlot, &TsnePlot::pointsSelected, ui->explanationForm->gradientsPlotBottom, &GradientsPlot::updateSelectedPoints);
    connect(ui->tsneForm->tsnePlot, &TsnePlot::zoomChanged, ui->overviewForm->overviewPlot, &OverviewPlot::updateZoomRectangle);

    isInitialized = true;
    show();
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

QList<qreal> AppWindow::jsonArrayToQList(const QJsonArray &jsonArray) {
    QList<qreal> list;
    for (const auto &value : jsonArray) {
        if (value.isDouble()) {
            list.append(value.toDouble());
        }
    }
    return list;
}

QList<QList<qreal>> AppWindow::jsonArrayToQList2D(const QJsonArray &jsonArray) {
    QList<QList<qreal>> listOfLists2D;

    for (const QJsonValue &outerValue : jsonArray) {
        if (outerValue.isArray()) {
            QJsonArray outerArray = outerValue.toArray();
            QList<qreal> innerList;

            for (const QJsonValue &value : std::as_const(outerArray)) {
                if (value.isDouble()) {
                    innerList.append(value.toDouble());
                }
            }

            listOfLists2D.append(innerList);
        }
    }

    return listOfLists2D;
}

QList<QList<QList<qreal>>> AppWindow::jsonArrayToQList3D(const QJsonArray &jsonArray) {
    QList<QList<QList<qreal>>> listOfLists3D;

    for (const QJsonValue &outerValue : jsonArray) {
        if (outerValue.isArray()) {
            QJsonArray outerArray = outerValue.toArray();
            QList<QList<qreal>> listOfLists2D;

            for (const QJsonValue &innerValue : std::as_const(outerArray)) {
                if (innerValue.isArray()) {
                    QJsonArray innerArray = innerValue.toArray();
                    QList<qreal> innerList;

                    for (const QJsonValue &value : std::as_const(innerArray)) {
                        if (value.isDouble()) {
                            innerList.append(value.toDouble());
                        }
                    }

                    listOfLists2D.append(innerList);
                }
            }

            listOfLists3D.append(listOfLists2D);
        }
    }

    return listOfLists3D;
}

AppWindow::~AppWindow()
{
    delete ui;
}
