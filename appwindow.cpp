#include "appwindow.h"
#include "qjsonobject.h"
#include "ui_appwindow.h"

#include <QJsonArray>
#include <QScatterSeries>
#include <QHorizontalBarSeries>
#include <QBarSet>

AppWindow::AppWindow(QJsonObject *json, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AppWindow)
{
    ui->setupUi(this);

    // Tsne Plot
    QJsonArray labels = json->take("labels").toArray();
    QStringList labelsList;
    std::transform(labels.begin(), labels.end(), std::back_inserter(labelsList),
                   [](const QJsonValue &value) { return value.toString(); });
    ui->tsneForm->tsnePlot->setLabels(labelsList);

    QJsonArray embedding = json->take("Y").toArray();
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
    ui->tsneForm->tsnePlot->setScatterSeries(tsneSeries);

    // Overview Plot
    QScatterSeries *overviewSeries = new QScatterSeries();
    for (const QPointF &point : tsneSeries->points()) {
        overviewSeries->append(point);
    }
    overviewSeries->setMarkerSize(tsneSeries->markerSize());
    // overviewSeries->setColor(tsneSeries->color());  // To keep the same style

    ui->overviewForm->overviewPlot->setScatterSeries(overviewSeries);


    // Explanation Plot
    QJsonArray featureNames = json->take("featureNames").toArray();
    QStringList featureNamesList;
    std::transform(featureNames.begin(), featureNames.end(), std::back_inserter(featureNamesList),
                   [](const QJsonValue &value) { return value.toString(); });
    ui->explanationForm->explanationPlot->setFeatureNames(featureNamesList);

    QJsonArray gradients = json->value("gradients").toArray();
    QList<QList<QList<qreal>>> *gradientsList = new QList<QList<QList<qreal>>>(convertJsonArrayToQList3D(gradients));
    ui->explanationForm->setGradients(gradientsList); // TODO MAYBE USELESS SO REMOVE ?

    QJsonArray meanGradients = json->value("mean_gradients").toArray();
    QList<qreal> meanGradientsList = jsonArrayToQList(meanGradients);
    // qDebug() << meanGradientsList;
    QHorizontalBarSeries *series = new QHorizontalBarSeries();
    QBarSet *set = new QBarSet("Mean Values");
    for (auto& value : meanGradientsList) {
        *set << value;
    }
    series->append(set);
    ui->explanationForm->explanationPlot->setHorizontalBarSeries(series);
}

QList<qreal> AppWindow::jsonArrayToQList(const QJsonArray &jsonArray) {
    QList<qreal> list;
    for (const auto &value : jsonArray) {
        if (value.isDouble()) {  // Ensure it's a number
            list.append(value.toDouble());
        }
    }
    return list;
}

QList<QList<QList<qreal>>> AppWindow::convertJsonArrayToQList3D(const QJsonArray &jsonArray) {
    QList<QList<QList<qreal>>> listOfLists3D;

    for (const QJsonValue &outerValue : jsonArray) {
        if (outerValue.isArray()) {
            QJsonArray outerArray = outerValue.toArray();
            QList<QList<qreal>> listOfLists2D;

            for (const QJsonValue &innerValue : outerArray) {
                if (innerValue.isArray()) {
                    QJsonArray innerArray = innerValue.toArray();
                    QList<qreal> innerList;

                    for (const QJsonValue &value : innerArray) {
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
