#include "appwindow.h"
#include "ui_appwindow.h"

#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QChartView>
#include <QValueAxis>
#include <QDebug>

AppWindow::AppWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AppWindow)
{
    ui->setupUi(this);

    QChart *tsneChart = new QChart();
    QScatterSeries *scatterSeries = new QScatterSeries();


    for (int i = 0; i < 100; ++i) {
        qreal x = QRandomGenerator::global()->bounded(100.0);
        qreal y = QRandomGenerator::global()->bounded(100.0);
        scatterSeries->append(x, y);
    }

    scatterSeries->setMarkerSize(8.0);

    tsneplot = new TsnePlot(tsneChart, scatterSeries);

    tsneChart->addSeries(scatterSeries);
    tsneChart->setTitle("Random Scatter Plot");

    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();
    axisX->setRange(0, 100);
    axisY->setRange(0, 100);
    tsneChart->addAxis(axisX, Qt::AlignBottom);
    tsneChart->addAxis(axisY, Qt::AlignLeft);
    scatterSeries->attachAxis(axisX);
    scatterSeries->attachAxis(axisY);

    if (!ui->centralwidget->layout()) {
        ui->centralwidget->setLayout(new QVBoxLayout());
    }

    ui->centralwidget->layout()->addWidget(tsneplot);
}

AppWindow::~AppWindow()
{
    delete ui;
}
