#include "barplot.h"

#include <QBarSet>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QHorizontalBarSeries>
#include <QBarSeries>

BarPlot::BarPlot(QChart *chart, QWidget *parent)
    : QChartView(chart, parent) {
    setRenderHint(QPainter::Antialiasing);

    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTheme(QChart::ChartThemeDark);
}

void BarPlot::initPlot(QAbstractBarSeries *series, Orientation orientation) {
    if (!series)
        return;

    // Remove old series if it exists
    if (barSeries && chart()->series().contains(barSeries)) {
        chart()->removeSeries(barSeries);
    }

    barSeries = series;
    chart()->addSeries(barSeries);

    // Remove old axes to prevent duplication
    QList<QAbstractAxis *> axes = chart()->axes();
    for (QAbstractAxis *axis : std::as_const(axes)) {
        chart()->removeAxis(axis);
    }

    // Calculate dynamic range for the value axis (X for horizontal, Y for vertical)
    qreal minValue = std::numeric_limits<qreal>::max();
    qreal maxValue = std::numeric_limits<qreal>::lowest();

    const auto &barSets = barSeries->barSets();
    for (int j = 0; j < barSets.size(); ++j) {
        const auto *set = barSets.at(j);
        for (int i = 0; i < set->count(); ++i) {
            qreal value = set->at(i);
            minValue = qMin(minValue, value);
            maxValue = qMax(maxValue, value);
        }
    }

    qreal padding = (maxValue - minValue) * 0.1; // 10% padding
    if (padding == 0) padding = 0.1; // Ensure some padding
    minValue -= padding;
    maxValue += padding;

    // Set up axes based on orientation
    if (orientation == Horizontal) {
        // X-axis is the value axis
        QValueAxis *axisX = new QValueAxis();
        axisX->setTitleText("Gradients"); // TODO
        axisX->setLabelFormat("%.2f");
        axisX->setTickCount(5);
        axisX->setRange(minValue, maxValue);
        axisX->setGridLineVisible(false);
        chart()->addAxis(axisX, Qt::AlignBottom);
        barSeries->attachAxis(axisX);

        // Y-axis is the category axis
        QCategoryAxis *axisY = new QCategoryAxis();
        for (int i = 0; i < labels.size(); ++i) {
            axisY->append(labels[i], i);
        }
        axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
        axisY->setGridLineVisible(false);
        chart()->addAxis(axisY, Qt::AlignLeft);
        barSeries->attachAxis(axisY);
    } else {
        // Y-axis is the value axis
        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("Gradients"); // TODO
        axisY->setLabelFormat("%.2f");
        axisY->setTickCount(5);
        axisY->setRange(minValue, maxValue);
        axisY->setGridLineVisible(false);
        chart()->addAxis(axisY, Qt::AlignLeft);
        barSeries->attachAxis(axisY);

        // X-axis is the category axis
        QCategoryAxis *axisX = new QCategoryAxis();
        for (int i = 0; i < labels.size(); ++i) {
            axisX->append(labels[i], i);
        }
        axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
        axisX->setGridLineVisible(false);
        chart()->addAxis(axisX, Qt::AlignBottom);
        barSeries->attachAxis(axisX);
    }

    currentOrientation = orientation;
}

void BarPlot::setLabels(const QStringList labels) {
    this->labels = labels;
}

void BarPlot::setMeanValues(QList<QList<qreal>> meanValues) {
    this->meanValues = meanValues;
}

void BarPlot::setValues(QList<QList<qreal>> values) {
    this->values = values;
}

void BarPlot::addValue(int id, const QColor &color, const QList<int> &indices) {
    if (indices.isEmpty()) {
        qDebug() << "No indices provided";
        return;
    } else if (values.isEmpty()) {
        qDebug() << "Gradients data is empty.";
        return;
    }

    QList<qreal> meanValues;
    int numFeatures = values.first().size();

    // Calculate mean values for the selected indices
    for (int i = 0; i < numFeatures; ++i) {
        qreal sum = 0;
        int count = 0;
        for (int index : indices) {
            if (index >= 0 && index < values.size()) {
                sum += values[index][i];
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

    idToBarSetMap[id] = newSet;

    // Append new data to the existing series
    if (barSeries) {
        barSeries->append(newSet);
        updateAxisRange(); // Update the axis range
    } else {
        delete newSet;  // Prevent memory leak if horizontalBarSeries is null
    }
}

void BarPlot::updateAxisRange() {
    if (!barSeries) return;

    qreal minValue = std::numeric_limits<qreal>::max();
    qreal maxValue = std::numeric_limits<qreal>::lowest();

    const auto &barSets = barSeries->barSets();
    for (int j = 0; j < barSets.size(); ++j) {
        const auto *set = barSets.at(j);
        for (int i = 0; i < set->count(); ++i) {
            qreal value = set->at(i);
            minValue = qMin(minValue, value);
            maxValue = qMax(maxValue, value);
        }
    }

    qreal padding = (maxValue - minValue) * 0.1; // 10% padding
    if (padding == 0) padding = 0.1; // Ensure some padding
    minValue -= padding;
    maxValue += padding;

    // Update the axis range based on the current orientation
    QValueAxis *axis = nullptr;
    if (currentOrientation == Horizontal) {
        axis = qobject_cast<QValueAxis *>(chart()->axes(Qt::Horizontal).first());
    } else {
        axis = qobject_cast<QValueAxis *>(chart()->axes(Qt::Vertical).first());
    }

    if (axis) {
        axis->setRange(minValue, maxValue);
    }

    chart()->update();
}

void BarPlot::setOrientation(Orientation orientation) {
    if (orientation == currentOrientation)
        return;

    // Create a new series based on the orientation
    QAbstractBarSeries *newSeries = nullptr;
    if (orientation == Horizontal) {
        newSeries = new QHorizontalBarSeries();
    } else {
        newSeries = new QBarSeries();
    }

    // Copy bar sets from the old series to the new one
    if (barSeries) {
        for (QBarSet *set : barSeries->barSets()) {
            newSeries->append(set);
        }
    }

    // Reinitialize the plot with the new series and orientation
    initPlot(newSeries, orientation);

    // Delete the old series
    if (barSeries) {
        delete barSeries;
    }
}

void BarPlot::removeValue(int id) {
    if (!barSeries || !idToBarSetMap.contains(id)) {
        qDebug() << "Invalid ID or series is empty.";
        return;
    }

    // Get the QBarSet corresponding to the ID
    QBarSet *barSet = idToBarSetMap.value(id);
    if (!barSet) {
        qDebug() << "Bar set not found for ID:" << id;
        return;
    }

    // Remove the bar set from the series
    barSeries->remove(barSet);

    // Remove the mapping
    idToBarSetMap.remove(id);

    // Update the axis range
    updateAxisRange();
}

// void BarPlot::removeValueByLabel(const QString &label) {
//     if (!barSeries) {
//         qDebug() << "Series is empty.";
//         return;
//     }

//     // Find the bar set with the specified label
//     const auto &barSets = barSeries->barSets();
//     for (int i = 0; i < barSets.size(); ++i) {
//         if (barSets.at(i)->label() == label) {
//             // Remove the bar set
//             removeValue(i);
//             return;
//         }
//     }

//     qDebug() << "Bar set with label" << label << "not found.";
// }
