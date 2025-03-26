#include "overviewplot.h"

#include <QChart>

OverviewPlot::OverviewPlot(QChart *chart, QWidget *parent)
    : QChartView(chart, parent), m_zoomRectItem(nullptr) {
    setRenderHint(QPainter::Antialiasing);

    chart->setAnimationOptions(QChart::SeriesAnimations);
    // chart->setTheme(QChart::ChartThemeDark);

    // Set the background color (#495759)
    QColor backgroundColor(0x49, 0x57, 0x59);

    // Chart background
    chart->setBackgroundBrush(QBrush(backgroundColor));

    // Plot area background
    chart->setPlotAreaBackgroundBrush(QBrush(Qt::lightGray));
    chart->setPlotAreaBackgroundVisible(true);

    // View background (for areas outside the chart)
    setBackgroundBrush(QBrush(backgroundColor));

    // Force white text for all chart elements
    QColor textColor = Qt::white;

    // Chart title
    chart->setTitleBrush(QBrush(textColor));

    // Legend
    if (chart->legend()) {
        chart->legend()->setLabelColor(textColor);
    }

    m_axisX->setLabelsColor(Qt::white);  // This affects only the numbers
    m_axisY->setLabelsColor(Qt::white);
}

void OverviewPlot::setScatterSeries(QScatterSeries *series) {
    if (!series || !chart()) return;

    // Remove the previous scatter series if it exists
    if (m_scatterSeries && chart()->series().contains(m_scatterSeries)) {
        chart()->removeSeries(m_scatterSeries);
    }

    m_scatterSeries = series;
    m_scatterSeries->setColor(Qt::gray);
    m_scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    m_scatterSeries->setBorderColor(Qt::black);
    chart()->addSeries(m_scatterSeries);

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

    m_axisX->setRange(minX - 1, maxX + 1); // Adding margins
    m_axisY->setRange(minY - 1, maxY + 1);
    m_axisX->setGridLineVisible(false);
    m_axisY->setGridLineVisible(false);

    // Remove existing axes if they exist
    QList<QAbstractAxis *> existingAxes = chart()->axes();
    for (QAbstractAxis *axis : std::as_const(existingAxes)) {
        if (axis) {
            chart()->removeAxis(axis);
            delete axis;
        }
    }

    chart()->addAxis(m_axisX, Qt::AlignBottom);
    chart()->addAxis(m_axisY, Qt::AlignLeft);

    // Attach the series to the new axes
    m_scatterSeries->attachAxis(m_axisX);
    m_scatterSeries->attachAxis(m_axisY);

    chart()->legend()->hide();
}

void OverviewPlot::updateZoomRectangle(qreal minX, qreal maxX, qreal minY, qreal maxY) {
    if (!chart() || !chart()->scene()) return;

    if (!m_zoomRectItem) {
        m_zoomRectItem = new QGraphicsRectItem();
        QPen pen(Qt::red);
        pen.setWidth(2);
        m_zoomRectItem->setPen(pen);
        chart()->scene()->addItem(m_zoomRectItem);
    }
    // Convert data coordinates to scene coordinates
    QPointF topLeft = chart()->mapToPosition(QPointF(minX, maxY));
    QPointF bottomRight = chart()->mapToPosition(QPointF(maxX, minY));

    QRectF visibleArea(topLeft, bottomRight);
    // qDebug() << zoomRectItem;
    // qDebug() << visibleArea;
    m_zoomRectItem->setRect(visibleArea);
}
