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
    setBackground(QColor(209, 209, 209));

    // Axis setup
    // QPen axisPen(Qt::white, 1);
    // xAxis->setBasePen(axisPen);
    // yAxis->setBasePen(axisPen);
    // xAxis->setTickPen(axisPen);
    // yAxis->setTickPen(axisPen);
    // xAxis->setSubTickPen(axisPen);
    // yAxis->setSubTickPen(axisPen);
    // xAxis->setTickLabelColor(Qt::white);
    // yAxis->setTickLabelColor(Qt::white);

    xAxis->setVisible(false);
    yAxis->setVisible(false);
    layer("grid")->setVisible(false);
    legend->setVisible(false);
}

void OverviewPlot::setScatterData(const QVector<double> &x, const QVector<double> &y) {
    clearPlottables(); // Clear existing data

    // Create and configure scatter plot
    QCPGraph *scatterPlot = addGraph();
    scatterPlot->setData(x, y);
    scatterPlot->setLineStyle(QCPGraph::lsNone);
    scatterPlot->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    scatterPlot->setPen(QPen(Qt::gray));
    scatterPlot->setBrush(QBrush(Qt::gray));

    // Calculate and set axis ranges with padding
    auto [xMin, xMax] = std::minmax_element(x.constBegin(), x.constEnd());
    auto [yMin, yMax] = std::minmax_element(y.constBegin(), y.constEnd());

    double xPadding = (*xMax - *xMin) * 0.2;
    double yPadding = (*yMax - *yMin) * 0.2;

    xAxis->setRange(*xMin - xPadding, *xMax + xPadding);
    yAxis->setRange(*yMin - yPadding, *yMax + yPadding);

    replot();
}

void OverviewPlot::updateZoomRectangle(double minX, double maxX, double minY, double maxY)
{
    if (!m_zoomRect) {
        m_zoomRect = new QCPItemRect(this);
        m_zoomRect->setPen(QPen(Qt::red, 2));
        m_zoomRect->setBrush(QBrush(QColor(255, 0, 0, 50)));
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
