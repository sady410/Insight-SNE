#include "explanationplot.h"
#include "qbarset.h"

#include <QValueAxis>
#include <QCategoryAxis>

ExplanationPlot::ExplanationPlot(QChart *chart, QWidget *parent)
    : QChartView(chart, parent) {
    setRenderHint(QPainter::Antialiasing);
}

void ExplanationPlot::setHorizontalBarSeries(QHorizontalBarSeries *series) {
    if (!series)
        return;

    if (horizontalBarSeries && chart()->series().contains(horizontalBarSeries)) {
        chart()->removeSeries(horizontalBarSeries);
    }

    horizontalBarSeries = series;
    chart()->addSeries(horizontalBarSeries);

    // Remove old axes to prevent duplication
    QList<QAbstractAxis *> axes = chart()->axes();
    for (QAbstractAxis *axis : std::as_const(axes)) {
        chart()->removeAxis(axis);
    }

    // **Calculate dynamic X-axis range**
    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::lowest();

    for (QBarSet *set : horizontalBarSeries->barSets()) { // Access QBarSet dynamically
        for (int i = 0; i < set->count(); ++i) {
            qreal value = set->at(i);
            minX = qMin(minX, value);
            maxX = qMax(maxX, value);
        }
    }

    qreal padding = (maxX - minX) * 0.1; // 10% padding
    if (padding == 0) padding = 0.1; // Ensure some padding
    minX -= padding;
    maxX += padding;

    // Set up X-axis
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Magnitudes");
    axisX->setLabelFormat("%.2f");
    axisX->setTickCount(5);
    axisX->setRange(minX, maxX);
    chart()->addAxis(axisX, Qt::AlignBottom);
    horizontalBarSeries->attachAxis(axisX);

    // Set up Y-axis
    QCategoryAxis *axisY = new QCategoryAxis();
    // axisY->setTitleText("Features");

    for (int i = 0; i < featureNames.size(); ++i) {
        axisY->append(featureNames[i], i);
    }

    axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    chart()->addAxis(axisY, Qt::AlignLeft);
    horizontalBarSeries->attachAxis(axisY);
}


void ExplanationPlot::setFeatureNames(const QStringList &featureNames) {
    this->featureNames = featureNames;
}


