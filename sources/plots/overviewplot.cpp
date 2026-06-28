#include "overviewplot.h"
#include <QDebug>
#include <QPainter>

OverviewPlot::OverviewPlot(QWidget *parent)
    : QCustomPlot(parent), m_zoomRect(nullptr)
{
    setupPlotAppearance();
}

void OverviewPlot::setupPlotAppearance()
{
    // Set dark theme background color
    setBackground(QColor(45, 45, 45)); // #2d2d2d

    // Configure plot margins
    setContentsMargins(5, 5, 5, 5);

    // Hide axis and grid for a clean look
    xAxis->setVisible(false);
    yAxis->setVisible(false);
    layer("grid")->setVisible(false);
    legend->setVisible(false);

    // Set the plot's border style
    QPen borderPen(QColor(68, 68, 68), 1); // #444444

    // Enable antialiasing for smoother drawing
    setAntialiasedElements(QCP::aeAll);
}

void OverviewPlot::setDataManager(DataManagerPtr dataManager)
{
    m_dataManager = dataManager;

    if (m_dataManager && m_dataManager->isDataLoaded()) {
        createScatterPlot();
    } else {
        qWarning() << "OverviewPlot: No data manager or data not loaded";
    }
}

void OverviewPlot::createScatterPlot()
{
    clearPlottables(); // Clear existing data

    // Get data from data manager (const references - no copying!)
    const QList<qreal>& xData = m_dataManager->xData();
    const QList<qreal>& yData = m_dataManager->yData();

    if (xData.isEmpty() || yData.isEmpty()) {
        qWarning() << "OverviewPlot: Empty coordinate data";
        return;
    }

    if (xData.size() != yData.size()) {
        qWarning() << "OverviewPlot: X and Y data size mismatch";
        return;
    }

    // Convert QList<qreal> to QVector<double> for QCustomPlot
    // Note: This is a necessary conversion for QCustomPlot API
    QVector<double> x(xData.size());
    QVector<double> y(yData.size());

    std::copy(xData.constBegin(), xData.constEnd(), x.begin());
    std::copy(yData.constBegin(), yData.constEnd(), y.begin());

    // Create and configure scatter plot
    QCPGraph *scatterPlot = addGraph();
    scatterPlot->setData(x, y);
    scatterPlot->setLineStyle(QCPGraph::lsNone);

    // Style the scatter points to match the theme
    QCPScatterStyle scatterStyle(QCPScatterStyle::ssCircle,
                                 QColor(76, 175, 80),
                                 QColor(170, 170, 170), 5);
    scatterPlot->setScatterStyle(scatterStyle);

    // Calculate and set axis ranges with padding
    auto [xMin, xMax] = std::minmax_element(x.constBegin(), x.constEnd());
    auto [yMin, yMax] = std::minmax_element(y.constBegin(), y.constEnd());

    // Add padding to avoid points being cut off at edges
    double xPadding = (*xMax - *xMin) * 0.2;
    double yPadding = (*yMax - *yMin) * 0.2;

    // If we have single points or identical values, make sure we still have a valid range
    if (xPadding == 0) xPadding = 1;
    if (yPadding == 0) yPadding = 1;

    xAxis->setRange(*xMin - xPadding, *xMax + xPadding);
    yAxis->setRange(*yMin - yPadding, *yMax + yPadding);

    replot();
}

void OverviewPlot::updateZoomRectangle(double minX, double maxX, double minY, double maxY)
{
    if (!m_zoomRect) {
        m_zoomRect = new QCPItemRect(this);
        // Style the zoom rectangle to match the theme
        QPen rectPen(QColor(76, 175, 80), 2); // #4CAF50 (green accent color)
        m_zoomRect->setPen(rectPen);
        // Semi-transparent green fill
        QColor fillColor = QColor(76, 175, 80, 50); // ~20% opacity
        m_zoomRect->setBrush(QBrush(fillColor));
    }

    m_zoomRect->topLeft->setCoords(minX, maxY);
    m_zoomRect->bottomRight->setCoords(maxX, minY);
    replot();
}

void OverviewPlot::resizeEvent(QResizeEvent *event)
{
    QCustomPlot::resizeEvent(event);
    // Maintain aspect ratio if needed
    // xAxis->setScaleRatio(yAxis, 1.0);
}
