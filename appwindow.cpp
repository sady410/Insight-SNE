#include "appwindow.h"
#include "qjsonobject.h"
#include "ui_appwindow.h"

#include <QJsonArray>
#include <QScatterSeries>
#include <QHorizontalBarSeries>

AppWindow::AppWindow(QJsonObject *json, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AppWindow)
{
    ui->setupUi(this);

    QJsonArray embedding = json->take("Y").toArray();
    QScatterSeries *scatterSeries = new QScatterSeries();

    for (const QJsonValue &val : std::as_const(embedding)) {
        QJsonArray point = val.toArray();
        if (point.size() == 2) {
            qreal x = point[0].toDouble();
            qreal y = point[1].toDouble();
            scatterSeries->append(x, y);
        }
    }

    scatterSeries->setMarkerSize(8.0);
    ui->tsneForm->setScatterSeries(scatterSeries);

    QJsonArray gradients = json->take("gradients").toArray();
    QList<QList<qreal>> *gradientsList = new QList<QList<qreal>>(convertJsonArrayToQList(gradients));

    ui->explanationForm->setGradients(gradientsList);
}

QList<QList<qreal>> AppWindow::convertJsonArrayToQList(const QJsonArray &jsonArray) {
    QList<QList<qreal>> listOfLists;

    for (const QJsonValue &value : jsonArray) {
        if (value.isArray()) {
            QList<qreal> innerList;
            QJsonArray innerArray = value.toArray();

            for (const QJsonValue &innerValue : std::as_const(innerArray)) {
                if (innerValue.isDouble()) {
                    innerList.append(innerValue.toDouble());
                }
            }
            listOfLists.append(innerList);
        }
    }

    return listOfLists;
}


AppWindow::~AppWindow()
{
    delete ui;
}
