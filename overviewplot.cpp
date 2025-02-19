#include "overviewplot.h"

#include <QChart>
#include <QValueAxis>

OverviewPlot::OverviewPlot(QChart *chart, QWidget *parent)
    : QChartView(chart, parent) {
    setRenderHint(QPainter::Antialiasing);
}

void OverviewPlot::setScatterSeries(QScatterSeries *series) {
    if (!series)
        return;

    // Remove the previous scatter series if it exists
    if (scatterSeries && chart()->series().contains(scatterSeries)) {
        chart()->removeSeries(scatterSeries);
    }

    scatterSeries = series;
    scatterSeries->setColor(Qt::gray);
    scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    scatterSeries->setBorderColor(Qt::black);
    chart()->addSeries(scatterSeries);

    // Compute min/max values dynamically
    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::min();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::min();

    for (int i = 0; i < series->count(); ++i) {
        QPointF point = series->at(i);
        minX = std::min(minX, point.x());
        maxX = std::max(maxX, point.x());
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }

    // Create and configure new axes
    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();
    axisX->setRange(minX - 1, maxX + 1); // Adding margins
    axisY->setRange(minY - 1, maxY + 1);

    // Remove existing axes if they exist
    QList<QAbstractAxis *> existingAxes = chart()->axes();
    for (QAbstractAxis *axis : std::as_const(existingAxes)) {
        chart()->removeAxis(axis);
        delete axis;
    }

    chart()->addAxis(axisX, Qt::AlignBottom);
    chart()->addAxis(axisY, Qt::AlignLeft);

    // Attach the series to the new axes
    scatterSeries->attachAxis(axisX);
    scatterSeries->attachAxis(axisY);

    chart()->legend()->hide();
}

