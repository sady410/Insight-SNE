#include "tsneplot.h"
#include <QGraphicsScene>
#include <QValueAxis>
#include <QDebug>
#include <QLineF>
#include <QLegendMarker>

TsnePlot::TsnePlot(QChart *chart, QWidget *parent)
    : QChartView(chart, parent), m_isDrawing(false), m_pathItem(nullptr), m_selectedPoints(new QList<QPointF>()), m_selectionCount(0) {
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);

    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTheme(QChart::ChartThemeDark);

    // Initialize the timer
    m_lassoFadeTimer = new QTimer(this);
    connect(m_lassoFadeTimer, &QTimer::timeout, this, &TsnePlot::fadeLasso);
    m_lassoOpacity = 1.0;
}

TsnePlot::~TsnePlot() {
    clearLasso();
    delete m_selectedPoints;
}

void TsnePlot::initPlot(QScatterSeries *series) {
    if (!series || series->count() == 0) {
        qDebug() << "Error: Empty series!";
        return;
    }

    // Clear previous scatter series
    for (auto it = m_scatterSeriesMap.begin(); it != m_scatterSeriesMap.end(); ++it) {
        QScatterSeries *s = it.value();
        if (chart()->series().contains(s)) {
            chart()->removeSeries(s);
            delete s;
        }
    }
    m_scatterSeriesMap.clear();
    m_pointIndexMap.clear();  // Clear previous mapping

    QStringList colorList = {"red", "blue", "green", "purple", "orange", "pink"};
    m_labelColors.clear(); // Clear previous label colors
    int colorIndex = 0;

    m_initialMinX = std::numeric_limits<qreal>::max();
    m_initialMaxX = std::numeric_limits<qreal>::min();
    m_initialMinY = std::numeric_limits<qreal>::max();
    m_initialMaxY = std::numeric_limits<qreal>::min();

    for (int i = 0; i < series->count(); ++i) {
        QPointF point = series->at(i);
        if (i >= m_labels.size()) {
            qDebug() << "Error: Label index out of bounds!";
            continue;
        }
        QString label = m_labels[i];

        if (!m_labelColors.contains(label)) {
            m_labelColors[label] = QColor(colorList[colorIndex % colorList.size()]);
            colorIndex++;
        }

        if (!m_scatterSeriesMap.contains(label)) {
            QScatterSeries *newSeries = new QScatterSeries();
            newSeries->setName(label);
            newSeries->setMarkerSize(8.0);
            newSeries->setColor(m_labelColors[label]);
            chart()->addSeries(newSeries);
            m_scatterSeriesMap[label] = newSeries;
        }

        m_scatterSeriesMap[label]->append(point);
        m_pointIndexMap[point] = i;  // Store original index

        m_initialMinX = std::min(m_initialMinX, point.x());
        m_initialMaxX = std::max(m_initialMaxX, point.x());
        m_initialMinY = std::min(m_initialMinY, point.y());
        m_initialMaxY = std::max(m_initialMaxY, point.y());
    }

    m_initialMinX -= 1;
    m_initialMaxX += 1;
    m_initialMinY -= 1;
    m_initialMaxY += 1;

    setupAxes(m_initialMinX, m_initialMaxX, m_initialMinY, m_initialMaxY);

    chart()->legend()->setVisible(true);
    chart()->legend()->setAlignment(Qt::AlignRight);
}

void TsnePlot::setLabels(const QStringList &labels) {
    this->m_labels = labels;
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

    for (QScatterSeries *series : std::as_const(m_scatterSeriesMap)) {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }
}

void TsnePlot::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_selectionCount < MAX_SELECTIONS) {
        clearLasso();  // Clear previous lasso if it exists

        m_isDrawing = true;
        QPointF scenePos = event->pos();
        m_path = QPainterPath(scenePos);
        m_firstPoint = scenePos;

        m_pathItem = new QGraphicsPathItem();
        m_pathItem->setPath(m_path);

        QPen pen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        m_pathItem->setPen(pen);

        chart()->scene()->addItem(m_pathItem);
    }
}

void TsnePlot::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDrawing) {
        QPointF scenePos = event->pos();
        m_path.lineTo(scenePos);
        m_pathItem->setPath(m_path);
    } else {
        // Check if the mouse is hovering over a point
        QPointF mousePos = event->pos();

        for (auto it = m_scatterSeriesMap.begin(); it != m_scatterSeriesMap.end(); ++it) {
            QScatterSeries *series = it.value();
            for (int i = 0; i < series->count(); ++i) {
                QPointF point = series->at(i);
                QPointF screenPos = chart()->mapToPosition(point);

                if (QLineF(mousePos, screenPos).length() < 5) {
                    QString label = series->name();
                    int id = m_pointIndexMap[point];
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
    if (event->button() == Qt::LeftButton && m_selectionCount < MAX_SELECTIONS) {
        m_isDrawing = false;
        m_path.lineTo(m_firstPoint);
        m_path.closeSubpath();
        m_pathItem->setPath(m_path);

        selectPointsInsideLasso();

        if (!m_selectedIndices.isEmpty()) {
            m_selectionCount++;
            emit pointsSelected(m_selectedIndices);
        }

        // Start the fade timer
        m_lassoFadeTimer->start(50);  // Adjust the interval as needed
    }
}

void TsnePlot::selectPointsInsideLasso() {
    m_selectedIndices.clear();
    m_selectedPoints->clear();

    for (auto it = m_scatterSeriesMap.begin(); it != m_scatterSeriesMap.end(); ++it) {
        QScatterSeries *series = it.value();
        for (int i = 0; i < series->count(); ++i) {
            QPointF point = series->at(i);
            QPointF screenPos = chart()->mapToPosition(point);

            if (m_path.contains(screenPos)) {
                m_selectedPoints->append(point);
                auto it = m_pointIndexMap.find(point);
                if (it != m_pointIndexMap.end()) {
                    m_selectedIndices.append(it->second);
                }
            }
        }
    }
}

void TsnePlot::fadeLasso() {
    if (m_pathItem) {
        m_lassoOpacity -= 0.05;  // Reduce opacity by 5% each time
        if (m_lassoOpacity <= 0.0) {
            m_lassoFadeTimer->stop();
            clearLasso();
            m_lassoOpacity = 1.0;  // Reset opacity for next lasso
        } else {
            QPen pen = m_pathItem->pen();
            QColor color = pen.color();
            color.setAlphaF(m_lassoOpacity);
            pen.setColor(color);
            m_pathItem->setPen(pen);
        }
    }
}

void TsnePlot::clearLasso() {
    if (m_pathItem) {
        chart()->scene()->removeItem(m_pathItem);
        delete m_pathItem;
        m_pathItem = nullptr;
        m_lassoFadeTimer->stop();
        m_lassoOpacity = 1.0;
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

    if (newMinX < m_initialMinX && newMaxX > m_initialMaxX) {
        newMinX = m_initialMinX;
        newMaxX = m_initialMaxX;
    }
    if (newMinY < m_initialMinY && newMaxY > m_initialMaxY) {
        newMinY = m_initialMinY;
        newMaxY = m_initialMaxY;
    }

    axisX->setRange(newMinX, newMaxX);
    axisY->setRange(newMinY, newMaxY);

    updateOverviewRectangle();
}

void TsnePlot::resetZoom() {
    setupAxes(m_initialMinX, m_initialMaxX, m_initialMinY, m_initialMaxY);
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

void TsnePlot::startHighlightSelection(const QColor &color, const QList<int>& indices) {

    chart()->setAnimationOptions(QChart::NoAnimation);
    // Clear any previous highlights
    // stopHighlightSelection();

    // Create a new series for highlighted points
    m_highlightedSeries = new QScatterSeries();
    m_highlightedSeries->setMarkerSize(12.0);  // Larger size for emphasis
    m_highlightedSeries->setColor(color);  // Use the provided highlight color
    m_highlightedSeries->setName("Highlighted");  // Set a name for the series (optional)

    // Iterate through all points and add highlighted points to the new series
    for (auto it = m_scatterSeriesMap.begin(); it != m_scatterSeriesMap.end(); ++it) {
        QScatterSeries *series = it.value();
        for (int i = 0; i < series->count(); ++i) {
            QPointF point = series->at(i);
            auto it = m_pointIndexMap.find(point);
            if (it != m_pointIndexMap.end()) {
                int index = it->second;
                if (indices.contains(index)) {
                    m_highlightedSeries->append(point);
                }
            }
        }
    }

    // Add the highlighted series to the chart
    chart()->addSeries(m_highlightedSeries);

    // Attach the series to the axes
    QValueAxis *axisX = qobject_cast<QValueAxis *>(chart()->axes(Qt::Horizontal).first());
    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart()->axes(Qt::Vertical).first());
    if (axisX && axisY) {
        m_highlightedSeries->attachAxis(axisX);
        m_highlightedSeries->attachAxis(axisY);
    }

    // Hide the legend marker for the highlighted series
    QList<QLegendMarker*> markers = chart()->legend()->markers(m_highlightedSeries);
    if (!markers.isEmpty()) {
        markers[0]->setVisible(false);  // Hide the legend marker
    }
}

void TsnePlot::stopHighlightSelection() {

    chart()->setAnimationOptions(QChart::SeriesAnimations);

    if (m_highlightedSeries) {
        // Remove the series from the chart (the chart will handle deletion)
        chart()->removeSeries(m_highlightedSeries);
        m_highlightedSeries = nullptr; // Reset the pointer to nullptr
    }

    // Restore the original series
    for (auto it = m_scatterSeriesMap.begin(); it != m_scatterSeriesMap.end(); ++it) {
        QScatterSeries *series = it.value();
        if (!chart()->series().contains(series)) {
            chart()->addSeries(series);
        }
    }

    // Restore the axes and legend
    setupAxes(m_initialMinX, m_initialMaxX, m_initialMinY, m_initialMaxY);
}
