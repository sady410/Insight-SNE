#include "gradientsplot.h"
#include "qbarset.h"

#include <QValueAxis>
#include <QCategoryAxis>

GradientsPlot::GradientsPlot(QChart *chart, QWidget *parent)
    : QChartView(chart, parent) {
    setRenderHint(QPainter::Antialiasing);

    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTheme(QChart::ChartThemeDark);
}

void GradientsPlot::initPlot(QHorizontalBarSeries *series) {
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

    const auto &barSets = horizontalBarSeries->barSets();
    for (int j = 0; j < barSets.size(); ++j) {
        const auto *set = barSets.at(j);
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
    axisX->setTitleText("Gradients"); // TODO
    axisX->setLabelFormat("%.2f");
    axisX->setTickCount(5);
    axisX->setRange(minX, maxX);
    axisX->setGridLineVisible(false);
    chart()->addAxis(axisX, Qt::AlignBottom);
    horizontalBarSeries->attachAxis(axisX);

    // Set up Y-axis
    QCategoryAxis *axisY = new QCategoryAxis();
    // axisY->setTitleText("Features");
    for (int i = 0; i < featureNames.size(); ++i) {
        axisY->append(featureNames[i], i);
    }
    axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    axisY->setGridLineVisible(false);
    chart()->addAxis(axisY, Qt::AlignLeft);
    horizontalBarSeries->attachAxis(axisY);
}

void GradientsPlot::setFeatureNames(const QStringList featureNames) {
    this->featureNames = featureNames;
}

void GradientsPlot::setMeanGradients(QList<QList<qreal>> meanGradients) {
    this->meanGradients = meanGradients;
}

void GradientsPlot::setGradients(QList<QList<qreal>> gradients) {
    this->gradients = gradients;
}

void GradientsPlot::updateSelectedPoints(const QList<int> &indices, const QColor &color) {
    addNewGradient(indices, color);
}

void GradientsPlot::addNewGradient(const QList<int> &indices, const QColor &color) {
    if (indices.isEmpty()) {
        qDebug() << "No indices provided";
        return;
    } else if (gradients.isEmpty()) {
        qDebug() << "Gradients data is empty.";
        return;
    }

    QList<qreal> meanValues;
    int numFeatures = gradients.first().size();

    // Calculate mean values for the selected indices
    for (int i = 0; i < numFeatures; ++i) {
        qreal sum = 0;
        int count = 0;
        for (int index : indices) {
            if (index >= 0 && index < gradients.size()) {
                sum += gradients[index][i];
                count++;
            }
        }
        meanValues.append(count > 0 ? sum / count : 0);
    }

    // Create a new bar set with the computed mean values
    QBarSet *newSet = new QBarSet("New Data"); // TODO: Customize the label
    for (qreal value : meanValues) {
        *newSet << value;
    }

    // Apply the color to the new bar set
    newSet->setColor(color);

    // Append new data to the existing series
    if (horizontalBarSeries) {
        horizontalBarSeries->append(newSet);

        // Recalculate the X-axis range
        qreal minX = std::numeric_limits<qreal>::max();
        qreal maxX = std::numeric_limits<qreal>::lowest();

        const auto &barSets = horizontalBarSeries->barSets();
        for (int j = 0; j < barSets.size(); ++j) {
            const auto *set = barSets.at(j);
            for (int i = 0; i < set->count(); ++i) {
                qreal value = set->at(i);
                minX = qMin(minX, value);
                maxX = qMax(maxX, value);
            }
        }

        // Add padding to the X-axis range
        qreal padding = (maxX - minX) * 0.1; // 10% padding
        if (padding == 0) padding = 0.1; // Ensure some padding
        minX -= padding;
        maxX += padding;

        // Update the X-axis range
        QValueAxis *axisX = qobject_cast<QValueAxis *>(chart()->axes(Qt::Horizontal).first());
        if (axisX) {
            axisX->setRange(minX, maxX);
        }

        chart()->update();
    } else {
        delete newSet;  // Prevent memory leak if horizontalBarSeries is null
    }
}
