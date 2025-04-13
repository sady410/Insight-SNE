#include "tsneplot.h"

TsnePlot::TsnePlot(QWidget *parent)
    : QCustomPlot(parent), m_colorMap(nullptr), m_colorScale(nullptr),
    m_highlightedGraph(nullptr), m_isDrawing(false),m_selectedPoints(new QList<QPointF>()), m_selectionCount(0)
{
    setInteractions(QCP::iRangeZoom);
    setMouseTracking(true);

    // Dark theme setup
    setBackground(QColor(53, 53, 53));

    xAxis->setVisible(false);
    yAxis->setVisible(false);

    layer("grid")->setVisible(false);

    // Legend setup
    legend->setVisible(true);
    legend->setBrush(QBrush(QColor(100, 100, 100, 200)));
    legend->setBorderPen(QPen(QColor(150, 150, 150, 200)));
    legend->setTextColor(Qt::white);

    // Initialize the timer
    m_lassoFadeTimer = new QTimer(this);
    connect(m_lassoFadeTimer, &QTimer::timeout, this, &TsnePlot::fadeLasso);
    m_lassoOpacity = 1.0;

    // Create custom layers (from bottom to top)
    addLayer("backgroundItems", layer("background"));
    addLayer("dataPoints", layer("main"));     // For regular scatter points
    addLayer("highlights", layer("main"));     // For highlighted points
    addLayer("arrows", layer("main"));         // For arrow vectors
    addLayer("lassoSelection", layer("overlay")); // For lasso selection

    // Set the current layer to dataPoints for default plotting
    setCurrentLayer("dataPoints");

    // Connect axis range changes
    connect(xAxis, QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged), this, &TsnePlot::handleZoomChanged);
    connect(yAxis, QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged), this, &TsnePlot::handleZoomChanged);
}

TsnePlot::~TsnePlot()
{
    clearLasso();
    delete m_selectedPoints;
}

void TsnePlot::setZValues(const QList<QList<qreal>> zValues)
{
    m_zValues = zValues;
}

void TsnePlot::setGradients(const QList<QList<QList<qreal>>> gradients)
{
    m_gradients = gradients;
}

void TsnePlot::setFeatureNames(const QStringList featureNames)
{
    m_featureNames = featureNames;
}

void TsnePlot::setLabels(const QStringList &labels)
{
    m_labels = labels;
}

void TsnePlot::initPlot(const QVector<double> &x, const QVector<double> &y)
{
    clearPlottables();
    clearItems();
    m_scatterGraphsMap.clear();
    m_pointIndexMap.clear();

    if (x.size() != y.size() || x.size() != m_labels.size()) {
        qDebug() << "Error: Data size mismatch!";
        return;
    }

    QList<QColor> colorList = {Qt::red, Qt::blue, Qt::green, Qt::yellow};
    m_labelColors.clear();
    int colorIndex = 0;

    // Initialize ranges
    m_initialMinX = *std::min_element(x.constBegin(), x.constEnd());
    m_initialMaxX = *std::max_element(x.constBegin(), x.constEnd());
    m_initialMinY = *std::min_element(y.constBegin(), y.constEnd());
    m_initialMaxY = *std::max_element(y.constBegin(), y.constEnd());

    // Add some padding
    qreal xRange = m_initialMaxX - m_initialMinX;
    qreal yRange = m_initialMaxY - m_initialMinY;
    m_initialMinX -= xRange * 0.1;
    m_initialMaxX += xRange * 0.1;
    m_initialMinY -= yRange * 0.1;
    m_initialMaxY += yRange * 0.1;

    // Group points by label
    QMap<QString, QVector<double>> labelX, labelY;
    for (int i = 0; i < x.size(); ++i) {
        QString label = m_labels[i];
        labelX[label] << x[i];
        labelY[label] << y[i];

        if (!m_labelColors.contains(label)) {
            m_labelColors[label] = colorList[colorIndex % colorList.size()];
            colorIndex++;
        }

        m_pointIndexMap[QPointF(x[i], y[i])] = i;
    }

    setCurrentLayer("dataPoints");

    // Create graphs for each label
    for (auto it = labelX.begin(); it != labelX.end(); ++it) {
        const QString &label = it.key();
        QCPGraph *graph = addGraph();
        graph->setName(label);
        graph->setData(labelX[label], labelY[label]);

        graph->setLayer("dataPoints");

        // This is the key line - removes the line from both plot and legend
        graph->setLineStyle(QCPGraph::lsNone);

        // Set scatter style (this will be shown in legend)
        QCPScatterStyle scatterStyle(QCPScatterStyle::ssCircle, 8);
        scatterStyle.setPen(QPen(m_labelColors[label]));
        scatterStyle.setBrush(QBrush(m_labelColors[label]));
        graph->setScatterStyle(scatterStyle);

        // Optional: improve rendering performance
        graph->setAntialiased(false);
        graph->setAdaptiveSampling(false);

        m_scatterGraphsMap[label] = graph;
    }

    // Set axis ranges
    xAxis->setRange(m_initialMinX, m_initialMaxX);
    yAxis->setRange(m_initialMinY, m_initialMaxY);

    replot();
}

QPointF TsnePlot::coordsToPixels(double x, double y) const
{
    return QPointF(xAxis->coordToPixel(x), yAxis->coordToPixel(y));
}

void TsnePlot::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_selectionCount < MAX_SELECTIONS) {
        clearLasso();

        m_isDrawing = true;
        m_lassoPoints.clear();
        m_lassoPoints << event->pos();

        // Convert screen position to data coordinates
        double xCoord = xAxis->pixelToCoord(event->pos().x());
        double yCoord = yAxis->pixelToCoord(event->pos().y());

        // Create a new lasso curve
        setCurrentLayer("lassoSelection");
        m_lassoCurve = new QCPCurve(xAxis, yAxis);
        m_lassoCurve->setLayer("lassoSelection");

        m_lassoCurve->removeFromLegend();

        // Set the pen for drawing
        QPen lassoPen(Qt::red, 2, Qt::SolidLine);
        m_lassoCurve->setPen(lassoPen);

        // Initialize with the first point (t=0, x, y)
        m_lassoCurve->addData(0, xCoord, yCoord);

        replot();
    }

    QCustomPlot::mousePressEvent(event);
}

void TsnePlot::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDrawing && m_lassoCurve) {
        // Convert screen position to data coordinates
        double xCoord = xAxis->pixelToCoord(event->pos().x());
        double yCoord = yAxis->pixelToCoord(event->pos().y());

        // Store point for selection calculation
        m_lassoPoints << event->pos();

        // Get the current point count and add new point with incremented t value
        int pointCount = m_lassoCurve->data()->size();
        m_lassoCurve->addData(pointCount, xCoord, yCoord);

        replot();
    } else {
        for (auto it = m_scatterGraphsMap.begin(); it != m_scatterGraphsMap.end(); ++it) {
            QCPGraph *graph = it.value();
            for (int i = 0; i < graph->dataCount(); ++i) {
                QPointF point(graph->dataMainKey(i), graph->dataMainValue(i));
                QPointF screenPos = coordsToPixels(point.x(), point.y());

                if (QLineF(event->pos(), screenPos).length() < 10) {
                    QString label = graph->name();
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

    QCustomPlot::mouseMoveEvent(event);
}

void TsnePlot::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isDrawing) {
        m_isDrawing = false;

        if (m_lassoPoints.size() > 2) {
            // Close the lasso by adding the first point again
            // Get coordinates of the first point
            QCPCurveDataContainer::const_iterator it = m_lassoCurve->data()->constBegin();
            if (it != m_lassoCurve->data()->constEnd()) {
                double firstX = it->key;
                double firstY = it->value;

                // Add as the last point with incremented t value
                int pointCount = m_lassoCurve->data()->size();
                m_lassoCurve->addData(pointCount, firstX, firstY);
            }

            // Also close the pixel points polygon
            m_lassoPoints << m_lassoPoints.first();

            selectPointsInsideLasso();

            if (!m_selectedIndices.isEmpty()) {
                m_selectionCount++;
                emit pointsSelected(m_selectedIndices);
            }

            // Start the fade animation
            m_lassoFadeTimer->start(50);
        }

        replot();
    }

    QCustomPlot::mouseReleaseEvent(event);
}

void TsnePlot::selectPointsInsideLasso()
{
    m_selectedIndices.clear();
    m_selectedPoints->clear();

    QPolygonF lassoPolygon;
    for (const QPoint &point : std::as_const(m_lassoPoints))     {
        lassoPolygon << point;
    }

    for (auto it = m_scatterGraphsMap.begin(); it != m_scatterGraphsMap.end(); ++it) {
        QCPGraph *graph = it.value();
        for (int i = 0; i < graph->dataCount(); ++i) {
            QPointF point(graph->dataMainKey(i), graph->dataMainValue(i));
            QPointF pixelPoint = coordsToPixels(point.x(), point.y());

            if (lassoPolygon.containsPoint(pixelPoint, Qt::OddEvenFill)) {
                m_selectedPoints->append(point);
                auto it = m_pointIndexMap.find(point);
                if (it != m_pointIndexMap.end()) {
                    m_selectedIndices.append(it->second);
                }
            }
        }
    }
}

void TsnePlot::fadeLasso()
{
    if (!m_lassoCurve) return;

    m_lassoOpacity -= 0.05;
    if (m_lassoOpacity <= 0.0) {
        m_lassoFadeTimer->stop();
        clearLasso();
        m_lassoOpacity = 1.0;
    } else {
        // Update opacity
        QPen pen = m_lassoCurve->pen();
        QColor color = pen.color();
        color.setAlphaF(m_lassoOpacity);
        pen.setColor(color);
        m_lassoCurve->setPen(pen);
        replot();
    }
}

void TsnePlot::clearLasso()
{
    // Remove the lasso curve
    if (m_lassoCurve) {
        removePlottable(m_lassoCurve);
        m_lassoCurve = nullptr;
    }

    m_lassoPoints.clear();
    m_lassoFadeTimer->stop();
    m_lassoOpacity = 1.0;
    replot();
}

void TsnePlot::wheelEvent(QWheelEvent *event)
{
    if (m_isDrawing) return;

    double factor = (event->angleDelta().y() > 0) ? 0.8 : 1.25;
    QPointF mousePos = event->position();
    double xCenter = xAxis->pixelToCoord(mousePos.x());
    double yCenter = yAxis->pixelToCoord(mousePos.y());

    double xRange = xAxis->range().size();
    double yRange = yAxis->range().size();

    double newXRange = xRange * factor;
    double newYRange = yRange * factor;

    double xLower = xCenter - newXRange * (xCenter - xAxis->range().lower) / xRange;
    double xUpper = xCenter + newXRange * (xAxis->range().upper - xCenter) / xRange;

    double yLower = yCenter - newYRange * (yCenter - yAxis->range().lower) / yRange;
    double yUpper = yCenter + newYRange * (yAxis->range().upper - yCenter) / yRange;

    // Clamp to initial bounds
    xLower = qMax(xLower, m_initialMinX);
    xUpper = qMin(xUpper, m_initialMaxX);
    yLower = qMax(yLower, m_initialMinY);
    yUpper = qMin(yUpper, m_initialMaxY);

    // Only apply if the range is valid
    if (xUpper > xLower && yUpper > yLower) {
        xAxis->setRange(xLower, xUpper);
        yAxis->setRange(yLower, yUpper);
        replot();
        updateOverviewRectangle();
    }
}

void TsnePlot::resetZoom()
{
    xAxis->setRange(m_initialMinX, m_initialMaxX);
    yAxis->setRange(m_initialMinY, m_initialMaxY);
    replot();
    updateOverviewRectangle();
}

void TsnePlot::handleZoomChanged(const QCPRange &newRange)
{
    Q_UNUSED(newRange)
    updateOverviewRectangle();

    // If we have an active contour feature, we may want to update the contour plot
    // with better resolution for the current view
    // For performance reasons, we might want to use a timer here to prevent too frequent updates
    // For now, we'll just call updateContourPlot directly
    updateContourPlot();
}

void TsnePlot::updateOverviewRectangle()
{
    emit zoomChanged(xAxis->range().lower, xAxis->range().upper,
                     yAxis->range().lower, yAxis->range().upper);
}

void TsnePlot::addArrows(int id, const QList<int> &indices, const QString &featureName, const QColor &color)
{
    // removeArrows(id); // Not needed

    int featureIndex = m_featureNames.indexOf(featureName);
    if (featureIndex == -1) {
        qDebug() << "Feature not found:" << featureName;
        return;
    }

    setCurrentLayer("arrows");

    for (int index : indices) {
        if (index < 0 || index >= m_gradients.size()) {
            qDebug() << "Index out of bounds:" << index;
            continue;
        }

        const QList<QList<qreal>>& featureVectors = m_gradients[index];
        if (featureVectors.size() < 2 || featureVectors[0].size() <= featureIndex) {
            qDebug() << "Invalid gradient data for index:" << index;
            continue;
        }

        QPointF startPoint;
        for (auto it = m_pointIndexMap.begin(); it != m_pointIndexMap.end(); ++it) {
            if (it->second == index) {
                startPoint = it->first;
                break;
            }
        }

        if (startPoint.isNull()) {
            qDebug() << "Point not found for index:" << index;
            continue;
        }

        // Create arrow line
        QCPItemLine *arrow = new QCPItemLine(this);
        arrow->start->setCoords(startPoint);
        arrow->end->setCoords(startPoint + QPointF(featureVectors[0][featureIndex],
                                                   featureVectors[1][featureIndex]));
        arrow->setHead(QCPLineEnding::esSpikeArrow);
        arrow->setPen(QPen(color, 2));

        arrow->setLayer("arrows");

        m_arrowItems.append(arrow);

        // Store arrow data
        ArrowData data;
        data.id = id;
        data.startPoint = startPoint;
        data.vector = QPointF(featureVectors[0][featureIndex], featureVectors[1][featureIndex]);
        data.featureName = featureName;
        data.color = color;
        m_arrowData.append(data);
    }

    replot();
}

void TsnePlot::removeArrows(int id)
{
    // First remove the visual arrow items
    for (int i = m_arrowItems.size() - 1; i >= 0; --i) {
        if (i < m_arrowData.size() && m_arrowData[i].id == id) {
            QCPItemLine* arrow = m_arrowItems[i];
            removeItem(arrow);
            m_arrowItems.removeAt(i);
        }
    }

    // Then remove the corresponding data entries
    for (int i = m_arrowData.size() - 1; i >= 0; --i) {
        if (m_arrowData[i].id == id) {
            m_arrowData.removeAt(i);
        }
    }

    replot();
}

void TsnePlot::startHighlightSelection(const QColor &color, const QList<int>& indices)
{
    // Remove previous highlight if exists
    if (m_highlightedGraph) {
        removePlottable(m_highlightedGraph);
        m_highlightedGraph = nullptr;
    }

    QVector<double> x, y;
    for (auto it = m_pointIndexMap.begin(); it != m_pointIndexMap.end(); ++it) {
        if (indices.contains(it->second)) {
            x << it->first.x();
            y << it->first.y();
        }
    }

    if (x.isEmpty()) return;

    setCurrentLayer("highlights");

    m_highlightedGraph = addGraph();
    m_highlightedGraph->setData(x, y);
    m_highlightedGraph->setLineStyle(QCPGraph::lsNone);
    m_highlightedGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
    m_highlightedGraph->setPen(QPen(color));
    m_highlightedGraph->setBrush(QBrush(color));

    // Hide from legend
    m_highlightedGraph->removeFromLegend();

    replot();
}

void TsnePlot::stopHighlightSelection()
{
    if (m_highlightedGraph) {
        removePlottable(m_highlightedGraph);
        m_highlightedGraph = nullptr;
        replot();
    }
}

void TsnePlot::drawContourPlot(const QString &featureName)
{
    int featureIndex = m_featureNames.indexOf(featureName);
    if (featureIndex == -1) {
        qDebug() << "Feature not found for contour plot:" << featureName;
        return;
    }

    // Clear existing contour plot if any
    clearContourPlot();

    // Set the current contour feature
    m_currentContourFeature = featureName;

    // Switch to background layer
    setCurrentLayer("backgroundItems");

    // Create a color map
    m_colorMap = new QCPColorMap(xAxis, yAxis);
    m_colorMap->setLayer("backgroundItems"); // Ensure it's in the background layer

    // Set interpolation for smooth contours
    m_colorMap->setInterpolate(true);

    // Generate data for the contour plot
    QCPColorMapData *contourData = createContourData(featureName);
    if (contourData) {
        m_colorMap->setData(contourData);
        delete contourData; // QCPColorMap takes ownership of the data

        // Set up the color gradient
        m_colorMap->setGradient(QCPColorGradient::gpGrayscale);

        // Rescale the data
        m_colorMap->rescaleDataRange(true);

        // Create a color scale
        m_colorScale = new QCPColorScale(this);
        plotLayout()->addElement(1, 0, m_colorScale);
        m_colorScale->setType(QCPAxis::atRight);
        m_colorMap->setColorScale(m_colorScale);
        m_colorScale->axis()->setLabel(featureName);

        // Adjust layout for color scale
        plotLayout()->setColumnStretchFactor(1, 0.2);

        // Create contour lines
        QCPColorMap *contourLines = new QCPColorMap(xAxis, yAxis);
        contourLines->setLayer("backgroundItems");
        contourLines->data()->setSize(m_colorMap->data()->keySize(), m_colorMap->data()->valueSize());
        contourLines->data()->setRange(m_colorMap->data()->keyRange(), m_colorMap->data()->valueRange());

        // Copy data from colorMap
        for (int x = 0; x < m_colorMap->data()->keySize(); ++x) {
            for (int y = 0; y < m_colorMap->data()->valueSize(); ++y) {
                contourLines->data()->setCell(x, y, m_colorMap->data()->cell(x, y));
            }
        }

        // Set up contour lines properties
        contourLines->setInterpolate(true);
        contourLines->setTightBoundary(true);
        contourLines->setGradient(QCPColorGradient::gpGrayscale);
        contourLines->setVisible(true);

        // Make the main color map semi-transparent
        m_colorMap->setInterpolate(true);
        m_colorMap->setTightBoundary(true);
        QBrush brush = m_colorMap->brush();
        QColor color = brush.color();
        color.setAlphaF(0.6); // 60% opacity
        brush.setColor(color);
        m_colorMap->setBrush(brush);
    }

    replot();
}

QCPColorMapData* TsnePlot::createContourData(const QString &featureName)
{
    int featureIndex = m_featureNames.indexOf(featureName);
    if (featureIndex == -1 || m_zValues.isEmpty()) {
        return nullptr;
    }

    // Determine grid size based on the amount of data
    int gridSize = qMin(300, qMax(50, static_cast<int>(sqrt(m_zValues.size()) * 2)));

    // Create data structure for contour map
    QCPColorMapData *data = new QCPColorMapData(gridSize, gridSize, QCPRange(m_initialMinX, m_initialMaxX), QCPRange(m_initialMinY, m_initialMaxY));

    // Get all data points
    QVector<QPointF> dataPoints;
    QVector<double> zValues;

    for (auto it = m_pointIndexMap.begin(); it != m_pointIndexMap.end(); ++it) {
        int index = it->second;
        if (index >= 0 && index < m_zValues.size() && featureIndex < m_zValues[index].size()) {
            dataPoints << it->first;
            zValues << m_zValues[index][featureIndex];
        }
    }

    if (dataPoints.isEmpty()) {
        delete data;
        return nullptr;
    }

    // Interpolate z values to grid points
    for (int xIndex = 0; xIndex < gridSize; ++xIndex) {
        for (int yIndex = 0; yIndex < gridSize; ++yIndex) {
            double x = data->keyRange().lower + (data->keyRange().upper - data->keyRange().lower) * xIndex / (gridSize - 1);
            double y = data->valueRange().lower + (data->valueRange().upper - data->valueRange().lower) * yIndex / (gridSize - 1);

            // Calculate an interpolated value using inverse distance weighting
            double totalWeight = 0;
            double weightedSum = 0;

            for (int i = 0; i < dataPoints.size(); ++i) {
                double distance = QLineF(x, y, dataPoints[i].x(), dataPoints[i].y()).length();

                if (distance < 1e-6) {
                    // We've hit an exact point, use that value
                    weightedSum = zValues[i];
                    totalWeight = 1;
                    break;
                }

                // Inverse distance weighting
                double weight = 1.0 / (distance * distance);
                weightedSum += weight * zValues[i];
                totalWeight += weight;
            }

            // Set the interpolated value
            double value = (totalWeight > 0) ? weightedSum / totalWeight : 0;
            data->setCell(xIndex, yIndex, value);
        }
    }

    return data;
}

void TsnePlot::clearContourPlot()
{
    // Remove color map
    if (m_colorMap) {
        removePlottable(m_colorMap);
        m_colorMap = nullptr;
    }

    // Remove color scale
    if (m_colorScale) {
        plotLayout()->remove(m_colorScale);
        m_colorScale = nullptr;

        // Reset layout
        plotLayout()->simplify();
    }

    m_currentContourFeature.clear();
    replot();
}

void TsnePlot::updateContourPlot()
{
    // If we have an active contour plot, redraw it with current view
    if (!m_currentContourFeature.isEmpty()) {
        drawContourPlot(m_currentContourFeature);
    }
}
