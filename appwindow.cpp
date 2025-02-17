#include "appwindow.h"
#include "qjsonobject.h"
#include "ui_appwindow.h"

#include <QJsonArray>
#include <QScatterSeries>

AppWindow::AppWindow(QJsonObject *json, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AppWindow)
{
    ui->setupUi(this);

    // Extract Y positions from the JSON object
    QJsonArray positionsArray = json->take("Y").toArray();
    QScatterSeries *scatterSeries = new QScatterSeries();

    // Populate the scatter series with data points
    for (const QJsonValue &val : positionsArray) {
        QJsonArray point = val.toArray();
        if (point.size() == 2) {
            qreal x = point[0].toDouble();
            qreal y = point[1].toDouble();
            scatterSeries->append(x, y);
        }
    }

    // Set the scatter series in the TsneForm
    scatterSeries->setMarkerSize(8.0);
    ui->tsneForm->setScatterSeries(scatterSeries);
}

AppWindow::~AppWindow()
{
    delete ui;
}
