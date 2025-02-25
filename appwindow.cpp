#include "appwindow.h"
#include "qjsonobject.h"
#include "ui_appwindow.h"

#include <QJsonArray>
#include <QScatterSeries>
#include <QHorizontalBarSeries>
#include <QBarSet>
#include <QValueAxis>

AppWindow::AppWindow(QJsonObject *json, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AppWindow)
{
    ui->setupUi(this);

    // Tsne Plot
    QJsonArray labels = json->value("labels").toArray();
    QStringList labelsList;
    std::transform(labels.begin(), labels.end(), std::back_inserter(labelsList),
                   [](const QJsonValue &value) { return value.toString(); });
    ui->tsneForm->tsnePlot->setLabels(labelsList);

    QJsonArray embedding = json->value("Y").toArray();
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

    // Explanation Plot
    QJsonArray featureNames = json->value("featureNames").toArray();
    QStringList featureNamesList;
    std::transform(featureNames.begin(), featureNames.end(), std::back_inserter(featureNamesList),
                   [](const QJsonValue &value) { return value.toString(); });
    ui->explanationForm->explanationPlot->setFeatureNames(featureNamesList);

    QJsonArray gradients = json->value("gradients").toArray();
    QList<QList<QList<qreal>>> gradientsList = convertJsonArrayToQList3D(gradients);
    ui->explanationForm->explanationPlot->setGradients(gradientsList);


    QJsonArray meanGradients = json->value("mean_gradients").toArray();
    QList<qreal> meanGradientsList = jsonArrayToQList(meanGradients);
    QHorizontalBarSeries *series = new QHorizontalBarSeries();
    QBarSet *set = new QBarSet("Mean Values");
    for (auto& value : meanGradientsList) {
        *set << value;
    }
    series->append(set);
    ui->explanationForm->explanationPlot->initPlot(series);

    QJsonArray magnitudes = json->value("magnitudes").toArray();
    QList<QList<qreal>> magnitudesList = jsonArrayToQList2D(magnitudes);
    ui->explanationForm->explanationPlot->setMagnitudes(magnitudesList);

    connect(ui->tsneForm->tsnePlot, &TsnePlot::pointsSelected, ui->explanationForm->explanationPlot, &ExplanationPlot::updateSelectedPoints);
    connect(ui->tsneForm->tsnePlot, &TsnePlot::zoomChanged, ui->overviewForm->overviewPlot, &OverviewPlot::updateZoomRectangle);
}

void AppWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

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

QList<QList<QList<qreal>>> AppWindow::convertJsonArrayToQList3D(const QJsonArray &jsonArray) {
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
