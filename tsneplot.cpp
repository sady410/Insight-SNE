#include "tsneplot.h"

TsnePlot::TsnePlot(QWidget *parent)
    : QCustomPlot(parent), m_isPanning(false), m_panStartPos(),
    m_colorMap(nullptr), m_colorScale(nullptr),m_zValues(), m_highlightedGraph(nullptr),
    m_isDrawing(false), m_selectedPoints(new QList<QPointF>()), m_selectionCount(0)
{
    setInteractions(QCP::iRangeZoom);
    setMouseTracking(true);

    // Dark theme styling
    setBackground(QColor(45, 45, 45));  // Dark background

    // Set explicit tooltip colors
    QPalette tooltipPalette;
    tooltipPalette.setColor(QPalette::ToolTipBase, QColor(45, 45, 45));  // Dark background
    tooltipPalette.setColor(QPalette::ToolTipText, Qt::white);          // White text
    QToolTip::setPalette(tooltipPalette);

    // Optional: Set font
    QFont tooltipFont("Arial", 10);
    tooltipFont.setBold(true);
    QToolTip::setFont(tooltipFont);

    // Hide axes
    xAxis->setVisible(false);
    yAxis->setVisible(false);

    // Hide grid
    layer("grid")->setVisible(false);

    // Enable antialiasing
    setNotAntialiasedElements(QCP::aeAll);
    // For performances
    setPlottingHints(QCP::phFastPolylines);

    // Legend styling
    legend->setVisible(true);
    legend->setBrush(QBrush(QColor(35, 35, 35, 230)));  // Darker than background
    legend->setBorderPen(QPen(QColor(68, 68, 68, 200)));
    legend->setTextColor(Qt::white);  // Light text for dark background
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

void TsnePlot::setAttributes(const QList<QList<qreal>> attributes)
{
    m_attributes = attributes;
}

void TsnePlot::setGradients(const QList<QList<QList<qreal>>> gradients)
{
    m_gradients = gradients;
}

void TsnePlot::setMagnitudes(const QList<QList<qreal>> magnitudes)
{
    m_magnitudes = magnitudes;
}

void TsnePlot::setFeatureNames(const QStringList featureNames)
{
    m_featureNames = featureNames;
}

void TsnePlot::setLabels(const QStringList &labels)
{
    m_labels = labels;
}

void TsnePlot::initPlot(const QVector<double> &x, const QVector<double> &y,
                        bool hasClassLabels, bool hasUniqueLabels)
{
    clearPlottables();
    clearItems();
    m_scatterGraphsMap.clear();
    m_pointIndexMap.clear();

    // Check if we have valid data
    if (x.size() != y.size()) {
        qDebug() << "Error: X and Y data size mismatch!";
        return;
    }

    // Handle the case where we have labels but size doesn't match
    if (hasClassLabels && x.size() != m_labels.size()) {
        qDebug() << "Error: Data and labels size mismatch!";
        return;
    }

    QList<QColor> colorList = {
        QColor(255, 87, 34),   // Deep Orange
        QColor(33, 150, 243),  // Blue
        QColor(76, 175, 80),   // Green
        QColor(255, 193, 7),   // Amber
        QColor(156, 39, 176),  // Purple
        QColor(0, 188, 212),   // Cyan
        QColor(233, 30, 99),   // Pink
        QColor(139, 195, 74)   // Light Green
    };

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

    setCurrentLayer("dataPoints");

    if (hasClassLabels) {
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

        // Create graphs for each label
        for (auto it = labelX.begin(); it != labelX.end(); ++it) {
            const QString &label = it.key();
            QCPGraph *graph = addGraph();
            graph->setName(label);
            graph->setData(labelX[label], labelY[label]);
            graph->setLayer("dataPoints");

            // Remove the line from both plot and legend
            graph->setLineStyle(QCPGraph::lsNone);

            // Set scatter style for each graph
            QCPScatterStyle scatterStyle(QCPScatterStyle::ssCircle, 8);
            scatterStyle.setPen(QPen(m_labelColors[label]));
            scatterStyle.setBrush(QBrush(m_labelColors[label]));
            graph->setScatterStyle(scatterStyle);

            // Optional: improve rendering performance
            graph->setAntialiased(false);
            graph->setAdaptiveSampling(false);
            m_scatterGraphsMap[label] = graph;
        }
    } else {
        // No class labels - create one graph for all points
        QCPGraph *graph = addGraph();
        graph->setName("Data Points");
        graph->setData(x, y);
        graph->setLayer("dataPoints");

        // Remove the line from both plot and legend
        graph->setLineStyle(QCPGraph::lsNone);

        // Set scatter style for the graph
        QCPScatterStyle scatterStyle(QCPScatterStyle::ssCircle, 8);
        scatterStyle.setPen(QPen(colorList[0]));
        scatterStyle.setBrush(QBrush(colorList[0]));
        graph->setScatterStyle(scatterStyle);

        // Optional: improve rendering performance
        graph->setAntialiased(false);
        graph->setAdaptiveSampling(false);

        // Map all points
        for (int i = 0; i < x.size(); ++i) {
            m_pointIndexMap[QPointF(x[i], y[i])] = i;
        }

        m_scatterGraphsMap["Data Points"] = graph;

        legend->setVisible(false);
    }

    // Add text labels for unique labels near each point if requested
    if (hasUniqueLabels && !m_labels.isEmpty()) {
        for (int i = 0; i < x.size(); ++i) {
            QCPItemText *textLabel = new QCPItemText(this);
            textLabel->setPositionAlignment(Qt::AlignCenter|Qt::AlignBottom);
            textLabel->position->setCoords(x[i], y[i] - 10); // Position above the point
            textLabel->setText(m_labels[i]);
            textLabel->setFont(QFont("Arial", 8));
            textLabel->setPen(QPen(Qt::black));
            textLabel->setLayer("dataPoints");
        }
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
    if (event->button() == Qt::RightButton) {
        // Start panning
        m_isPanning = true;
        m_panStartPos = event->pos();
        setCursor(Qt::ClosedHandCursor); // Change cursor to indicate panning
        event->accept();
        return;
    }
    else if (event->button() == Qt::LeftButton && m_selectionCount < MAX_SELECTIONS) {
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

        // Set the pen for drawing with accent color
        QPen lassoPen(QColor(76, 175, 80), 2, Qt::SolidLine);  // Green accent
        m_lassoCurve->setPen(lassoPen);

        // Initialize with the first point (t=0, x, y)
        m_lassoCurve->addData(0, xCoord, yCoord);

        replot();
    }

    QCustomPlot::mousePressEvent(event);
}

void TsnePlot::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPanning) {
        // Calculate how much the mouse has moved by
        QPoint delta = event->pos() - m_panStartPos;

        // Convert pixel movement to coordinate movement
        double deltaX = xAxis->pixelToCoord(m_panStartPos.x()) - xAxis->pixelToCoord(m_panStartPos.x() + delta.x());
        double deltaY = yAxis->pixelToCoord(m_panStartPos.y()) - yAxis->pixelToCoord(m_panStartPos.y() + delta.y());

        // Calculate new range values
        double newLower = xAxis->range().lower + deltaX;
        double newUpper = xAxis->range().upper + deltaX;
        double newLowerY = yAxis->range().lower + deltaY;
        double newUpperY = yAxis->range().upper + deltaY;

        // Check X boundaries
        if (newLower < m_initialMinX) {
            // Adjust both to maintain range size
            double adjustment = m_initialMinX - newLower;
            newLower = m_initialMinX;
            newUpper += adjustment;
        }

        if (newUpper > m_initialMaxX) {
            // Adjust both to maintain range size
            double adjustment = newUpper - m_initialMaxX;
            newUpper = m_initialMaxX;
            newLower -= adjustment;
        }

        // Check Y boundaries
        if (newLowerY < m_initialMinY) {
            // Adjust both to maintain range size
            double adjustment = m_initialMinY - newLowerY;
            newLowerY = m_initialMinY;
            newUpperY += adjustment;
        }

        if (newUpperY > m_initialMaxY) {
            // Adjust both to maintain range size
            double adjustment = newUpperY - m_initialMaxY;
            newUpperY = m_initialMaxY;
            newLowerY -= adjustment;
        }

        // Set the new ranges
        xAxis->setRange(newLower, newUpper);
        yAxis->setRange(newLowerY, newUpperY);

        // Update the start position for the next move event
        m_panStartPos = event->pos();

        // Update the plot
        replot();
        updateOverviewRectangle();
        return;
    }
    else if (m_isDrawing && m_lassoCurve) {
        double xCoord = xAxis->pixelToCoord(event->pos().x());
        double yCoord = yAxis->pixelToCoord(event->pos().y());

        // Store point for selection calculation
        m_lassoPoints << event->pos();

        // Get the current point count and add new point with incremented t value
        int pointCount = m_lassoCurve->data()->size();
        m_lassoCurve->addData(pointCount, xCoord, yCoord);

        m_lassoCurve->layer()->replot();
    }

    // Tooltip handling
    if (!m_isPanning && !m_isDrawing) {

        bool pointFound = false;

        for (auto graph : m_scatterGraphsMap) {
            for (int i = 0; i < graph->dataCount(); ++i) {
                QPointF point(graph->dataMainKey(i), graph->dataMainValue(i));
                QPointF screenPos = coordsToPixels(point.x(), point.y());

                if (QLineF(event->pos(), screenPos).length() < 15) {
                    auto it = m_pointIndexMap.find(point);
                    if (it != m_pointIndexMap.end()) {
                        QString tooltipText = QString("<span style='color:white;'>"  // Force white text
                                                      "Label: %1<br>"
                                                      "Coordinates: (%2, %3)<br>"
                                                      "ID: %4"
                                                      "</span>")
                                                  .arg(graph->name())
                                                  .arg(point.x(), 0, 'f', 2)
                                                  .arg(point.y(), 0, 'f', 2)
                                                  .arg(it->second);

                        QToolTip::showText(event->globalPos(), tooltipText, this,
                                           QRect(), 3000);  // Show for 3 seconds
                        pointFound = true;
                        break;
                    }
                }
            }
            if (pointFound) break;
        }

        if (!pointFound) {
            QToolTip::hideText();
        }
    }

    QCustomPlot::mouseMoveEvent(event);
}

void TsnePlot::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton && m_isPanning) {
        // End panning
        m_isPanning = false;
        setCursor(Qt::ArrowCursor); // Reset cursor
        event->accept();
        return;
    }
    else if (event->button() == Qt::LeftButton && m_isDrawing) {
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

        m_lassoCurve->layer()->replot();
    }

    QCustomPlot::mouseReleaseEvent(event);
}

void TsnePlot::selectPointsInsideLasso()
{
    m_selectedIndices.clear();
    m_selectedPoints->clear();

    QPolygonF lassoPolygon;
    // can be optimized using a more efficient spatial data structure (quad tree or grid)
    // to pre-filter points that might be in the lasso area
    // QRectF lassoRect = lassoPolygon.boundingRect();
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

    // updateContourPlot();
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
        QColor arrowColor = color.isValid() ? color : QColor(76, 175, 80);  // Default to green accent
        arrow->setPen(QPen(arrowColor, 2));

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

void TsnePlot::changeArrowsColor(int id, const QColor &newColor)
{
    // First, check if there are any arrows with this ID
    bool arrowsFound = false;
    for (int i = 0; i < m_arrowData.size(); ++i) {
        if (m_arrowData[i].id == id) {
            arrowsFound = true;
            break;
        }
    }

    if (!arrowsFound) {
        qDebug() << "No arrows found with id:" << id;
        return;
    }

    // Collect all arrow data with this ID and update their color
    QList<ArrowData> arrowsToRedraw;
    for (int i = 0; i < m_arrowData.size(); ++i) {
        if (m_arrowData[i].id == id) {
            // Update the color in the data structure
            m_arrowData[i].color = newColor;
            arrowsToRedraw.append(m_arrowData[i]);
        }
    }

    // Remove all existing arrow items with this ID
    for (int i = m_arrowItems.size() - 1; i >= 0; --i) {
        if (i < m_arrowData.size() && m_arrowData[i].id == id) {
            QCPItemLine* arrow = m_arrowItems[i];
            removeItem(arrow);
            m_arrowItems.removeAt(i);
        }
    }

    // Redraw all arrows with the new color
    setCurrentLayer("arrows");
    for (const ArrowData &data : arrowsToRedraw) {
        QCPItemLine *arrow = new QCPItemLine(this);
        arrow->start->setCoords(data.startPoint);
        arrow->end->setCoords(data.startPoint + data.vector);
        arrow->setHead(QCPLineEnding::esSpikeArrow);
        arrow->setPen(QPen(newColor, 2));
        arrow->setLayer("arrows");
        m_arrowItems.append(arrow);
    }

    // Replot to show the changes
    replot();
}

void TsnePlot::startHighlightSelection(const QColor &color, const QList<int>& indices)
{
    // Remove previous highlight if exists
    if (m_highlightedGraph) {
        removePlottable(m_highlightedGraph);
        m_highlightedGraph = nullptr;
    }

    m_highlightedIndices = indices;
    m_highlightColor = color;

    // First, we'll create temporary shadowed graphs for each original graph
    QMap<QString, QCPGraph*> shadowGraphs;
    for (auto it = m_scatterGraphsMap.begin(); it != m_scatterGraphsMap.end(); ++it) {
        QString label = it.key();
        QCPGraph *originalGraph = it.value();

        // Create a new graph for shadowed points (with same data)
        QCPGraph *shadowGraph = addGraph();
        shadowGraph->setData(originalGraph->data());
        shadowGraph->setLineStyle(QCPGraph::lsNone);

        // Create a shadowed style
        QCPScatterStyle shadowStyle = originalGraph->scatterStyle();
        QColor shadowColor = m_labelColors[label];
        shadowColor.setAlphaF(0.3); // Reduce opacity
        shadowStyle.setPen(QPen(shadowColor));
        shadowStyle.setBrush(QBrush(shadowColor));
        shadowGraph->setScatterStyle(shadowStyle);

        // Hide shadowed graph from legend
        shadowGraph->removeFromLegend();

        shadowGraphs[label] = shadowGraph;

        // Hide the original graph
        originalGraph->setVisible(false);
    }

    // Store the shadow graphs
    m_shadowGraphs = shadowGraphs;

    // Create highlighted points graph for each category
    for (auto it = m_scatterGraphsMap.begin(); it != m_scatterGraphsMap.end(); ++it) {
        QString label = it.key();
        QCPGraph *originalGraph = it.value();

        // Collect highlighted points for this category
        QVector<double> x, y;
        for (int i = 0; i < originalGraph->dataCount(); ++i) {
            QPointF point(originalGraph->dataMainKey(i), originalGraph->dataMainValue(i));
            auto indexIt = m_pointIndexMap.find(point);
            if (indexIt != m_pointIndexMap.end() && indices.contains(indexIt->second)) {
                x << point.x();
                y << point.y();
            }
        }

        if (!x.isEmpty()) {
            // Create a new graph for highlighted points
            QCPGraph *highlightGraph = addGraph();
            highlightGraph->setData(x, y);
            highlightGraph->setLineStyle(QCPGraph::lsNone);

            // Use original style but with the highlight color
            QCPScatterStyle highlightStyle = originalGraph->scatterStyle();
            highlightStyle.setPen(QPen(color));
            highlightStyle.setBrush(QBrush(color));
            highlightGraph->setScatterStyle(highlightStyle);

            // Hide from legend
            highlightGraph->removeFromLegend();

            // Store for later cleanup
            m_highlightGraphs.append(highlightGraph);
        }
    }

    replot();
}

void TsnePlot::stopHighlightSelection()
{
    // Remove highlighted graphs
    for (QCPGraph* graph : std::as_const(m_highlightGraphs)) {
        removePlottable(graph);
    }
    m_highlightGraphs.clear();

    // Remove shadow graphs
    for (QCPGraph* graph : std::as_const(m_shadowGraphs)) {
        removePlottable(graph);
    }
    m_shadowGraphs.clear();

    // Make original graphs visible again
    for (auto it = m_scatterGraphsMap.begin(); it != m_scatterGraphsMap.end(); ++it) {
        it.value()->setVisible(true);
    }

    m_highlightedIndices.clear();

    replot();
}

void TsnePlot::drawContourPlot(const QString &featureName, const QString &attributesOrMagnitudes) {
    // Make sure we completely clean up first
    clearContourPlot();

    // Add a small delay to ensure UI is updated properly
    QApplication::processEvents();

    int featureIndex = m_featureNames.indexOf(featureName);
    if (featureIndex == -1) {
        qDebug() << "Feature not found:" << featureName;
        return;
    }

    // Set z-values based on mode
    m_zValues = (attributesOrMagnitudes == "Attributes") ? m_attributes : m_magnitudes;
    m_currentContourType = attributesOrMagnitudes;
    m_currentContourFeature = featureName;

    // Create color map
    setCurrentLayer("backgroundItems");
    m_colorMap = new QCPColorMap(xAxis, yAxis);
    m_colorMap->setInterpolate(true);

    // Assign data
    QCPColorMapData *contourData = createContourData(featureName);
    if (!contourData) {
        qDebug() << "Failed to create contour data";
        delete m_colorMap;
        m_colorMap = nullptr;
        return;
    }

    // Get actual range from contour data
    double minCellValue = std::numeric_limits<double>::max();
    double maxCellValue = std::numeric_limits<double>::lowest();
    for (int x = 0; x < contourData->keySize(); ++x) {
        for (int y = 0; y < contourData->valueSize(); ++y) {
            double cellValue = contourData->cell(x, y);
            minCellValue = qMin(minCellValue, cellValue);
            maxCellValue = qMax(maxCellValue, cellValue);
        }
    }
    // qDebug() << "Using contour data range:" << minCellValue << "to" << maxCellValue;

    // Assign data to color map
    m_colorMap->setData(contourData, true);

    // Set up gradient
    QCPColorGradient gradient(QCPColorGradient::gpGrayscale);
    m_colorMap->setGradient(gradient);

    if (qAbs(maxCellValue - minCellValue) < 1e-6) {
        maxCellValue = minCellValue + 1.0;
    }
    m_colorMap->setDataRange(QCPRange(minCellValue, maxCellValue));

    // Make sure we have space for the color scale in the layout
    if (plotLayout()->elementCount() > 1 && plotLayout()->hasElement(0, 1)) {
        // If position is occupied, add another column
        plotLayout()->insertColumn(1);
    }

    // Style the color scale
    m_colorScale = new QCPColorScale(this);
    plotLayout()->addElement(0, 1, m_colorScale);
    m_colorScale->setType(QCPAxis::atRight);
    m_colorScale->axis()->setLabel(featureName);
    m_colorScale->axis()->setLabelColor(Qt::white);  // White text
    m_colorScale->axis()->setTickLabelColor(Qt::white);  // White tick labels
    m_colorScale->setGradient(gradient);
    m_colorScale->setDataRange(QCPRange(minCellValue, maxCellValue));

    // Connect color map to scale
    m_colorMap->setColorScale(m_colorScale);

    // Force complete replot
    replot(QCustomPlot::rpQueuedReplot);
}

QCPColorMapData* TsnePlot::createContourData(const QString &featureName) {
    int featureIndex = m_featureNames.indexOf(featureName);
    if (featureIndex == -1 || m_zValues.isEmpty()) {
        qDebug() << "Feature not found or no z-values available:" << featureName;
        return nullptr;
    }

    // Determine grid size (adjust as needed)
    int gridSize = 100;

    // Create data structure with initial axis ranges
    QCPColorMapData *data = new QCPColorMapData(
        gridSize, gridSize,
        QCPRange(m_initialMinX, m_initialMaxX),
        QCPRange(m_initialMinY, m_initialMaxY)
        );

    // Collect valid data points and z-values
    QVector<QPointF> dataPoints;
    QVector<double> zValues;

    double minZ = std::numeric_limits<double>::max();
    double maxZ = std::numeric_limits<double>::lowest();

    for (auto it = m_pointIndexMap.begin(); it != m_pointIndexMap.end(); ++it) {
        int pointIndex = it->second;
        if (pointIndex >= 0 && pointIndex < m_zValues.size() &&
            featureIndex < m_zValues[pointIndex].size()) {
            dataPoints << it->first;
            double zVal = m_zValues[pointIndex][featureIndex];
            zValues << zVal;

            // Track min/max z values
            minZ = qMin(minZ, zVal);
            maxZ = qMax(maxZ, zVal);
        }
    }

    // qDebug() << "Original Z value range:" << minZ << "to" << maxZ;

    if (zValues.isEmpty()) {
        qDebug() << "No valid data points for contour plot";
        delete data;
        return nullptr;
    }

    // If we have very little variation in z values, artificially increase it
    if (qAbs(maxZ - minZ) < 1e-6) {
        qDebug() << "WARNING: Z values have almost no variation, artificially adding range";
        for (int i = 0; i < zValues.size(); ++i) {
            zValues[i] = minZ + (i * (maxZ - minZ + 0.1) / zValues.size());
        }
    }

    // Fill grid with interpolated values
    for (int xIndex = 0; xIndex < gridSize; ++xIndex) {
        for (int yIndex = 0; yIndex < gridSize; ++yIndex) {
            double x = data->keyRange().lower + xIndex * data->keyRange().size() / (gridSize - 1);
            double y = data->valueRange().lower + yIndex * data->valueRange().size() / (gridSize - 1);

            double totalWeight = 0.0;
            double weightedSum = 0.0;
            double minDist = std::numeric_limits<double>::max();
            double nearestValue = 0.0;

            for (int i = 0; i < dataPoints.size(); ++i) {
                double distance = QLineF(x, y, dataPoints[i].x(), dataPoints[i].y()).length();

                // Keep track of nearest point as fallback
                if (distance < minDist) {
                    minDist = distance;
                    nearestValue = zValues[i];
                }

                if (distance < 1e-6) {
                    // Direct hit on a point
                    weightedSum = zValues[i];
                    totalWeight = 1.0;
                    break;
                }

                // Use inverse distance weighting
                double weight = 1.0 / (distance * distance);
                weightedSum += weight * zValues[i];
                totalWeight += weight;
            }

            // Assign interpolated value
            double value;
            if (totalWeight > 0) {
                value = weightedSum / totalWeight;
            } else {
                // Fallback to nearest point if weights are zero
                value = nearestValue;
            }

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

    // Remove color scale more carefully
    if (m_colorScale) {
        // Check if the color scale is still in the layout before removing it
        for (int i = 0; i < plotLayout()->elementCount(); ++i) {
            if (plotLayout()->elementAt(i) == m_colorScale) {
                plotLayout()->take(m_colorScale);
                break;
            }
        }
        delete m_colorScale;
        m_colorScale = nullptr;
    }

    // Reset the current contour feature
    m_currentContourFeature.clear();
    m_currentContourType.clear();

    // Make sure to reset any layout settings that might have been changed
    plotLayout()->simplify();

    // Force a complete replot
    replot(QCustomPlot::rpQueuedReplot);
}

void TsnePlot::updateContourPlot()
{
    // If we have an active contour plot, redraw it with current view
    if (!m_currentContourFeature.isEmpty() && m_colorMap != nullptr) {
        // Instead of completely redrawing, just update the data for the current view
        int featureIndex = m_featureNames.indexOf(m_currentContourFeature);
        if (featureIndex == -1) return;

        // Get current axis ranges
        QCPRange xRange = xAxis->range();
        QCPRange yRange = yAxis->range();

        // Create new contour data with current view bounds
        QCPColorMapData *newData = createContourDataForView(m_currentContourFeature, xRange, yRange);
        if (!newData) return;

        // Update the color map with new data
        m_colorMap->setData(newData, true);

        // No need to change the data range, gradient or color scale
        // Just replot to show the updated view
        replot();
    }
}

QCPColorMapData* TsnePlot::createContourDataForView(const QString &featureName, const QCPRange &xRange, const QCPRange &yRange)
{
    int featureIndex = m_featureNames.indexOf(featureName);
    if (featureIndex == -1 || m_zValues.isEmpty()) {
        return nullptr;
    }

    // Use higher resolution for zoomed views
    int gridSize = 100;

    // Create data structure with current view ranges
    QCPColorMapData *data = new QCPColorMapData(
        gridSize, gridSize,
        xRange, yRange
        );

    // Collect data points and z-values for interpolation
    QVector<QPointF> dataPoints;
    QVector<double> zValues;

    for (auto it = m_pointIndexMap.begin(); it != m_pointIndexMap.end(); ++it) {
        int pointIndex = it->second;

        // Only include points that are within or near the current view
        // to improve performance and quality
        if (pointIndex >= 0 && pointIndex < m_zValues.size() &&
            featureIndex < m_zValues[pointIndex].size()) {

            // Add padding to include points just outside view for better interpolation
            const double padding = 0.2; // 20% padding
            double padXRange = xRange.size() * padding;
            double padYRange = yRange.size() * padding;

            if (it->first.x() >= xRange.lower - padXRange &&
                it->first.x() <= xRange.upper + padXRange &&
                it->first.y() >= yRange.lower - padYRange &&
                it->first.y() <= yRange.upper + padYRange) {

                dataPoints << it->first;
                zValues << m_zValues[pointIndex][featureIndex];
            }
        }
    }

    if (zValues.isEmpty()) {
        qDebug() << "No valid data points for contour plot in current view";
        delete data;
        return nullptr;
    }

    // Fill grid with interpolated values
    for (int xIndex = 0; xIndex < gridSize; ++xIndex) {
        for (int yIndex = 0; yIndex < gridSize; ++yIndex) {
            double x = data->keyRange().lower + xIndex * data->keyRange().size() / (gridSize - 1);
            double y = data->valueRange().lower + yIndex * data->valueRange().size() / (gridSize - 1);

            double totalWeight = 0.0;
            double weightedSum = 0.0;
            double minDist = std::numeric_limits<double>::max();
            double nearestValue = 0.0;

            for (int i = 0; i < dataPoints.size(); ++i) {
                double distance = QLineF(x, y, dataPoints[i].x(), dataPoints[i].y()).length();

                // Keep track of nearest point as fallback
                if (distance < minDist) {
                    minDist = distance;
                    nearestValue = zValues[i];
                }

                if (distance < 1e-6) {
                    // Direct hit on a point
                    weightedSum = zValues[i];
                    totalWeight = 1.0;
                    break;
                }

                // Use inverse distance weighting
                double weight = 1.0 / (distance * distance);
                weightedSum += weight * zValues[i];
                totalWeight += weight;
            }

            // Assign interpolated value
            double value;
            if (totalWeight > 0) {
                value = weightedSum / totalWeight;
            } else {
                // Fallback to nearest point if weights are zero
                value = nearestValue;
            }

            data->setCell(xIndex, yIndex, value);
        }
    }

    return data;
}
