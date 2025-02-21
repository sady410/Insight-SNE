#include "tsneplot.h"

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QPainterPath>
#include <QPen>
#include <QDebug>
#include <QValueAxis>

TsnePlot::TsnePlot(QChart *chart, QWidget *parent)
    : QChartView(chart, parent), isDrawing(false), pathItem(nullptr), selectedPoints(new QList<QPointF>()) {
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);

    chart->setAnimationOptions(QChart::SeriesAnimations);
}

void TsnePlot::setScatterSeries(QScatterSeries *series) {
    if (!series || series->count() == 0) {
        qDebug() << "Error: Empty series!";
        return;
    }

    // Clear previous scatter series
    for (QScatterSeries *s : std::as_const(scatterSeriesMap)) {
        if (chart()->series().contains(s)) {
            chart()->removeSeries(s);
            delete s;
        }
    }
    scatterSeriesMap.clear();

    // Define colors
    QStringList colorList = {"red", "blue", "green", "purple", "orange", "pink"};
    QMap<QString, QColor> labelColors;
    int colorIndex = 0;

    // Compute initial min/max values dynamically
    initialMinX = std::numeric_limits<qreal>::max();
    initialMaxX = std::numeric_limits<qreal>::min();
    initialMinY = std::numeric_limits<qreal>::max();
    initialMaxY = std::numeric_limits<qreal>::min();

    // Iterate over points and create series per label
    for (int i = 0; i < series->count(); ++i) {
        QPointF point = series->at(i);
        if (i >= labels.size()) {
            qDebug() << "Error: Label index out of bounds!";
            continue;
        }
        QString label = labels[i];

        // Assign a unique color to each label
        if (!labelColors.contains(label)) {
            labelColors[label] = QColor(colorList[colorIndex % colorList.size()]);
            colorIndex++;
        }

        // Create a new series if label does not exist
        if (!scatterSeriesMap.contains(label)) {
            QScatterSeries *newSeries = new QScatterSeries();
            newSeries->setName(label);
            newSeries->setMarkerSize(8.0);
            newSeries->setColor(labelColors[label]);
            chart()->addSeries(newSeries);  // **Ensure series is added before setting axes**
            scatterSeriesMap[label] = newSeries;
        }

        // Add point to respective series
        scatterSeriesMap[label]->append(point);

        // Update min/max values
        initialMinX = std::min(initialMinX, point.x());
        initialMaxX = std::max(initialMaxX, point.x());
        initialMinY = std::min(initialMinY, point.y());
        initialMaxY = std::max(initialMaxY, point.y());
    }

    // Expand slightly to allow space around the points
    initialMinX -= 1;
    initialMaxX += 1;
    initialMinY -= 1;
    initialMaxY += 1;

    // Remove existing axes if they exist
    QList<QAbstractAxis *> existingAxes = chart()->axes();
    for (QAbstractAxis *axis : std::as_const(existingAxes)) {
        chart()->removeAxis(axis);
        delete axis;
    }

    // Create and configure new axes
    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();
    axisX->setRange(initialMinX, initialMaxX);
    axisY->setRange(initialMinY, initialMaxY);

    chart()->addAxis(axisX, Qt::AlignBottom);
    chart()->addAxis(axisY, Qt::AlignLeft);

    // Attach each series to the new axes
    for (QScatterSeries *labelSeries : std::as_const(scatterSeriesMap)) {
        labelSeries->attachAxis(axisX);
        labelSeries->attachAxis(axisY);
    }

    // Enable legend
    chart()->legend()->setVisible(true);
    chart()->legend()->setAlignment(Qt::AlignRight);

    // qDebug() << "Scatter series set with " << scatterSeriesMap.size() << " series.";
}

void TsnePlot::setLabels(const QStringList &labels) {
    this->labels = labels;
}

void TsnePlot::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        // Clear the previous lasso if it exists
        if (pathItem) {
            chart()->scene()->removeItem(pathItem);
            delete pathItem;
            pathItem = nullptr;  // Reset pathItem
        }

        // Clear selected points when starting a new selection
        selectedPoints->clear();

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

    // Compute new zoomed ranges
    qreal newMinX = mouseChartPos.x() - (mouseChartPos.x() - minX) * factor;
    qreal newMaxX = mouseChartPos.x() + (maxX - mouseChartPos.x()) * factor;
    qreal newMinY = mouseChartPos.y() - (mouseChartPos.y() - minY) * factor;
    qreal newMaxY = mouseChartPos.y() + (maxY - mouseChartPos.y()) * factor;

    // Clamp zoom-out to initial limits
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

    qDebug() << "Zooming to mouse position: " << mouseChartPos
             << " New X range: [" << newMinX << ", " << newMaxX << "]"
             << " New Y range: [" << newMinY << ", " << newMaxY << "]";

    updateOverviewRectangle();
}

void TsnePlot::resetZoom() {
    QValueAxis *axisX = qobject_cast<QValueAxis *>(chart()->axes(Qt::Horizontal).first());
    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart()->axes(Qt::Vertical).first());

    if (!axisX || !axisY) {
        qDebug() << "Error: Axes are null!";
        return;
    }

    axisX->setRange(initialMinX, initialMaxX);
    axisY->setRange(initialMinY, initialMaxY);

    qDebug() << "Reset zoom to initial range: X(" << initialMinX << "," << initialMaxX
             << ") Y(" << initialMinY << "," << initialMaxY << ")";

    updateOverviewRectangle();
}


void TsnePlot::updateOverviewRectangle() {
    if (!chart() || !chart()->scene()) {
        qDebug() << "Error: chart() or chart()->scene() is null!";
        return;
    }

    QList<QAbstractAxis *> axes = chart()->axes();
    if (axes.isEmpty()) {
        qDebug() << "Error: No axes found!";
        return;
    }

    QValueAxis *axisX = qobject_cast<QValueAxis *>(chart()->axes(Qt::Horizontal).first());
    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart()->axes(Qt::Vertical).first());

    if (!axisX || !axisY) {
        qDebug() << "Error: One or both axes are null!";
        return;
    }

    emit zoomChanged(axisX->min(), axisX->max(), axisY->min(), axisY->max());
    qDebug() << "zoomChanged emitted: X(" << axisX->min() << "," << axisX->max()
             << ") Y(" << axisY->min() << "," << axisY->max() << ")";
}


void TsnePlot::mouseMoveEvent(QMouseEvent *event) {
    if (isDrawing) {
        QPointF scenePos = event->pos();
        path.lineTo(scenePos);
        pathItem->setPath(path);
    }
}

void TsnePlot::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDrawing = false;
        path.lineTo(firstPoint);
        path.closeSubpath();
        pathItem->setPath(path);
        selectPointsInsideLasso();
    }
}

void TsnePlot::selectPointsInsideLasso() {
    qDebug() << "Checking points inside lasso...";

    // Iterate through all the scatter series
    for (auto it = scatterSeriesMap.begin(); it != scatterSeriesMap.end(); ++it) {
        QScatterSeries *series = it.value();
        for (int i = 0; i < series->count(); ++i) {
            QPointF point = series->at(i);
            QPointF screenPos = chart()->mapToPosition(point);

            // Check if the point is inside the lasso path
            if (path.contains(screenPos)) {
                qDebug() << "Point inside lasso:" << point;
                // Add the point to the selectedpoints list
                selectedPoints->append(point);
            }
        }
    }

    qDebug() << "Total points selected: " << selectedPoints->size();
}

