#include "tsneplot.h"
#include <QGraphicsScene>
#include <QValueAxis>
#include <QDebug>
#include <QLineF>
#include <QLegendMarker>

TsnePlot::TsnePlot(QChart *chart, QWidget *parent)
    : QChartView(chart, parent), isDrawing(false), pathItem(nullptr), selectedPoints(new QList<QPointF>()), selectionCount(0) {
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);

    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTheme(QChart::ChartThemeDark);

    // Initialize the timer
    lassoFadeTimer = new QTimer(this);
    connect(lassoFadeTimer, &QTimer::timeout, this, &TsnePlot::fadeLasso);
    lassoOpacity = 1.0;
}

TsnePlot::~TsnePlot() {
    clearLasso();
    delete selectedPoints;
}

void TsnePlot::initPlot(QScatterSeries *series) {
    if (!series || series->count() == 0) {
        qDebug() << "Error: Empty series!";
        return;
    }

    // Clear previous scatter series
    for (auto it = scatterSeriesMap.begin(); it != scatterSeriesMap.end(); ++it) {
        QScatterSeries *s = it.value();
        if (chart()->series().contains(s)) {
            chart()->removeSeries(s);
            delete s;
        }
    }
    scatterSeriesMap.clear();
    pointIndexMap.clear();  // Clear previous mapping

    QStringList colorList = {"red", "blue", "green", "purple", "orange", "pink"};
    labelColors.clear(); // Clear previous label colors
    int colorIndex = 0;

    initialMinX = std::numeric_limits<qreal>::max();
    initialMaxX = std::numeric_limits<qreal>::min();
    initialMinY = std::numeric_limits<qreal>::max();
    initialMaxY = std::numeric_limits<qreal>::min();

    for (int i = 0; i < series->count(); ++i) {
        QPointF point = series->at(i);
        if (i >= labels.size()) {
            qDebug() << "Error: Label index out of bounds!";
            continue;
        }
        QString label = labels[i];

        if (!labelColors.contains(label)) {
            labelColors[label] = QColor(colorList[colorIndex % colorList.size()]);
            colorIndex++;
        }

        if (!scatterSeriesMap.contains(label)) {
            QScatterSeries *newSeries = new QScatterSeries();
            newSeries->setName(label);
            newSeries->setMarkerSize(8.0);
            newSeries->setColor(labelColors[label]);
            chart()->addSeries(newSeries);
            scatterSeriesMap[label] = newSeries;
        }

        scatterSeriesMap[label]->append(point);
        pointIndexMap[point] = i;  // Store original index

        initialMinX = std::min(initialMinX, point.x());
        initialMaxX = std::max(initialMaxX, point.x());
        initialMinY = std::min(initialMinY, point.y());
        initialMaxY = std::max(initialMaxY, point.y());
    }

    initialMinX -= 1;
    initialMaxX += 1;
    initialMinY -= 1;
    initialMaxY += 1;

    setupAxes(initialMinX, initialMaxX, initialMinY, initialMaxY);

    chart()->legend()->setVisible(true);
    chart()->legend()->setAlignment(Qt::AlignRight);
}

void TsnePlot::setLabels(const QStringList &labels) {
    this->labels = labels;
}

void TsnePlot::setupAxes(qreal minX, qreal maxX, qreal minY, qreal maxY) {
    QList<QAbstractAxis *> existingAxes = chart()->axes();
    for (QAbstractAxis *axis : std::as_const(existingAxes)) {
        chart()->removeAxis(axis);
        delete axis;
    }

    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();
    axisX->setRange(minX, maxX);
    axisY->setRange(minY, maxY);

    axisX->setGridLineVisible(false);
    axisY->setGridLineVisible(false);

    chart()->addAxis(axisX, Qt::AlignBottom);
    chart()->addAxis(axisY, Qt::AlignLeft);

    for (QScatterSeries *series : std::as_const(scatterSeriesMap)) {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }
}

void TsnePlot::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && selectionCount < MAX_SELECTIONS) {
        clearLasso();  // Clear previous lasso if it exists

        isDrawing = true;
        QPointF scenePos = event->pos();
        path = QPainterPath(scenePos);
        firstPoint = scenePos;

        pathItem = new QGraphicsPathItem();
        pathItem->setPath(path);

        QPen pen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        pathItem->setPen(pen);

        chart()->scene()->addItem(pathItem);
    }
}

void TsnePlot::mouseMoveEvent(QMouseEvent *event) {
    if (isDrawing) {
        QPointF scenePos = event->pos();
        path.lineTo(scenePos);
        pathItem->setPath(path);
    } else {
        // Check if the mouse is hovering over a point
        QPointF mousePos = event->pos();

        for (auto it = scatterSeriesMap.begin(); it != scatterSeriesMap.end(); ++it) {
            QScatterSeries *series = it.value();
            for (int i = 0; i < series->count(); ++i) {
                QPointF point = series->at(i);
                QPointF screenPos = chart()->mapToPosition(point);

                if (QLineF(mousePos, screenPos).length() < 5) {
                    QString label = series->name();
                    int id = pointIndexMap[point];
                    QString tooltipText = QString("Label: %1\nCoordinates: (%2, %3)\nID: %4")
                                              .arg(label)
                                              .arg(point.x(), 0, 'f', 2)
                                              .arg(point.y(), 0, 'f', 2)
                                              .arg(id);

                    QToolTip::showText(event->globalPosition().toPoint(), tooltipText, this);
                    return;
                }
            }
        }

        QToolTip::hideText();
    }
}

void TsnePlot::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && selectionCount < MAX_SELECTIONS) {
        isDrawing = false;
        path.lineTo(firstPoint);
        path.closeSubpath();
        pathItem->setPath(path);

        selectPointsInsideLasso();

        if (!selectedIndices.isEmpty()) {
            selectionCount++;
            emit pointsSelected(selectedIndices);
        }

        // Start the fade timer
        lassoFadeTimer->start(50);  // Adjust the interval as needed
    }
}

void TsnePlot::selectPointsInsideLasso() {
    selectedIndices.clear();
    selectedPoints->clear();

    for (auto it = scatterSeriesMap.begin(); it != scatterSeriesMap.end(); ++it) {
        QScatterSeries *series = it.value();
        for (int i = 0; i < series->count(); ++i) {
            QPointF point = series->at(i);
            QPointF screenPos = chart()->mapToPosition(point);

            if (path.contains(screenPos)) {
                selectedPoints->append(point);
                auto it = pointIndexMap.find(point);
                if (it != pointIndexMap.end()) {
                    selectedIndices.append(it->second);
                }
            }
        }
    }
}

void TsnePlot::fadeLasso() {
    if (pathItem) {
        lassoOpacity -= 0.05;  // Reduce opacity by 5% each time
        if (lassoOpacity <= 0.0) {
            lassoFadeTimer->stop();
            clearLasso();
            lassoOpacity = 1.0;  // Reset opacity for next lasso
        } else {
            QPen pen = pathItem->pen();
            QColor color = pen.color();
            color.setAlphaF(lassoOpacity);
            pen.setColor(color);
            pathItem->setPen(pen);
        }
    }
}

void TsnePlot::clearLasso() {
    if (pathItem) {
        chart()->scene()->removeItem(pathItem);
        delete pathItem;
        pathItem = nullptr;
        lassoFadeTimer->stop();
        lassoOpacity = 1.0;
    }
}

void TsnePlot::wheelEvent(QWheelEvent *event) {
    if (!chart()) {
        qDebug() << "Error: chart() is null!";
        return;
    }

    QRectF plotArea = chart()->plotArea();
    QPointF mousePos = event->position();

    if (!plotArea.contains(mousePos)) {
        return;
    }

    const double zoomFactor = 1.2;
    double factor = (event->angleDelta().y() > 0) ? zoomFactor : (1.0 / zoomFactor);

    QPointF mouseChartPos = chart()->mapToValue(mousePos);

    QValueAxis *axisX = qobject_cast<QValueAxis *>(chart()->axes(Qt::Horizontal).first());
    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart()->axes(Qt::Vertical).first());

    if (!axisX || !axisY) {
        qDebug() << "Error: Axes are null!";
        return;
    }

    qreal minX = axisX->min();
    qreal maxX = axisX->max();
    qreal minY = axisY->min();
    qreal maxY = axisY->max();

    qreal newMinX = mouseChartPos.x() - (mouseChartPos.x() - minX) * factor;
    qreal newMaxX = mouseChartPos.x() + (maxX - mouseChartPos.x()) * factor;
    qreal newMinY = mouseChartPos.y() - (mouseChartPos.y() - minY) * factor;
    qreal newMaxY = mouseChartPos.y() + (maxY - mouseChartPos.y()) * factor;

    if (newMinX < initialMinX && newMaxX > initialMaxX) {
        newMinX = initialMinX;
        newMaxX = initialMaxX;
    }
    if (newMinY < initialMinY && newMaxY > initialMaxY) {
        newMinY = initialMinY;
        newMaxY = initialMaxY;
    }

    axisX->setRange(newMinX, newMaxX);
    axisY->setRange(newMinY, newMaxY);

    updateOverviewRectangle();
}

void TsnePlot::resetZoom() {
    setupAxes(initialMinX, initialMaxX, initialMinY, initialMaxY);
    updateOverviewRectangle();
}

void TsnePlot::updateOverviewRectangle() {
    if (!chart() || !chart()->scene()) {
        qDebug() << "Error: chart() or chart()->scene() is null!";
        return;
    }

    QValueAxis *axisX = qobject_cast<QValueAxis *>(chart()->axes(Qt::Horizontal).first());
    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart()->axes(Qt::Vertical).first());

    if (!axisX || !axisY) {
        qDebug() << "Error: One or both axes are null!";
        return;
    }

    emit zoomChanged(axisX->min(), axisX->max(), axisY->min(), axisY->max());
}

void TsnePlot::startHighlightSelection(const QColor &highlightColor, const QList<int>& indices) {
    // Clear any previous highlights
    stopHighlightSelection();

    // Create a new series for highlighted points
    highlightedSeries = new QScatterSeries();
    highlightedSeries->setMarkerSize(12.0);  // Larger size for emphasis
    highlightedSeries->setColor(highlightColor);  // Use the provided highlight color
    highlightedSeries->setName("Highlighted");  // Set a name for the series (optional)

    // Iterate through all points and add highlighted points to the new series
    for (auto it = scatterSeriesMap.begin(); it != scatterSeriesMap.end(); ++it) {
        QScatterSeries *series = it.value();
        for (int i = 0; i < series->count(); ++i) {
            QPointF point = series->at(i);
            auto it = pointIndexMap.find(point);
            if (it != pointIndexMap.end()) {
                int index = it->second;
                if (indices.contains(index)) {
                    highlightedSeries->append(point);
                }
            }
        }
    }

    // Add the highlighted series to the chart
    chart()->addSeries(highlightedSeries);

    // Attach the series to the axes
    QValueAxis *axisX = qobject_cast<QValueAxis *>(chart()->axes(Qt::Horizontal).first());
    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart()->axes(Qt::Vertical).first());
    if (axisX && axisY) {
        highlightedSeries->attachAxis(axisX);
        highlightedSeries->attachAxis(axisY);
    }

    // Hide the legend marker for the highlighted series
    QList<QLegendMarker*> markers = chart()->legend()->markers(highlightedSeries);
    if (!markers.isEmpty()) {
        markers[0]->setVisible(false);  // Hide the legend marker
    }
}

void TsnePlot::stopHighlightSelection() {
    if (highlightedSeries) {
        // Remove the series from the chart (the chart will handle deletion)
        chart()->removeSeries(highlightedSeries);
        highlightedSeries = nullptr; // Reset the pointer to nullptr
    }

    // Restore the original series
    for (auto it = scatterSeriesMap.begin(); it != scatterSeriesMap.end(); ++it) {
        QScatterSeries *series = it.value();
        if (!chart()->series().contains(series)) {
            chart()->addSeries(series);
        }
    }

    // Restore the axes and legend
    setupAxes(initialMinX, initialMaxX, initialMinY, initialMaxY);
}
