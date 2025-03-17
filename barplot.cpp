#include "barplot.h"
#include <QBarSet>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QHorizontalBarSeries>
#include <QBarSeries>
#include <QDebug>

BarPlot::BarPlot(QChart *chart, QWidget *parent)
    : QChartView(chart, parent), barSeries(nullptr) {
    setRenderHint(QPainter::Antialiasing);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTheme(QChart::ChartThemeDark);
}

BarPlot::~BarPlot() {
    cleanupOldSeries();
}

void BarPlot::initPlot(QAbstractBarSeries *series, Orientation orientation) {
    if (!series) return;

    cleanupOldSeries();
    cleanupOldAxes();

    barSeries = series;
    chart()->addSeries(barSeries);

    setupAxes(orientation);
    currentOrientation = orientation;
}

void BarPlot::setOrientation(Orientation orientation) {
    if (orientation == currentOrientation) return;

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

void BarPlot::setLabels(const QStringList &labels) {
    this->labels = labels;
}

void BarPlot::setValues(const QList<QList<qreal>> &values) {
    this->values = values;
}

void BarPlot::setMeanValues(const QList<QList<qreal>> &meanValues) {
    this->meanValues = meanValues;
}

void BarPlot::addValue(int id, const QColor &color, const QList<int> &indices) {
    if (indices.isEmpty()) {
        qDebug() << "No indices provided";
        return;
    }
    if (values.isEmpty()) {
        qDebug() << "Values data is empty.";
        return;
    }

    QList<qreal> meanValues;
    int numFeatures = values.first().size();

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

    QBarSet *newSet = new QBarSet("New Data");
    for (qreal value : meanValues) {
        *newSet << value;
    }
    newSet->setColor(color);

    // Store the initial color of the bar set
    initialBarSetColors[id] = color;

    idToBarSetMap[id] = newSet;

    if (barSeries) {
        barSeries->append(newSet);
        updateAxisRange(); // Update the axis range after adding new data
    } else {
        delete newSet;
    }
}

void BarPlot::removeValue(int id) {
    if (!barSeries || !idToBarSetMap.contains(id)) {
        qDebug() << "Invalid ID or series is empty.";
        return;
    }

    QBarSet *barSet = idToBarSetMap.value(id);
    if (!barSet) {
        qDebug() << "Bar set not found for ID:" << id;
        return;
    }

    barSeries->remove(barSet);
    idToBarSetMap.remove(id);
    originalBarSetColors.remove(barSet); // Remove the original color from the map
    updateAxisRange();
}

void BarPlot::updateAxisRange() {
    if (!barSeries) {
        qDebug() << "Bar series is null in updateAxisRange";
        return;
    }

    qreal minValue, maxValue;
    calculateDynamicRange(minValue, maxValue);

    qreal padding = (maxValue - minValue) * 0.1; // 10% padding
    if (padding == 0) padding = 0.1;

    // Apply padding to maxValue
    maxValue += padding;

    // Only apply negative padding to minValue if there are negative values
    if (minValue < 0) {
        minValue -= padding;
    } else {
        // Ensure the axis starts at zero if all values are positive
        minValue = 0;
    }

    QValueAxis *axis = nullptr;

    if (currentOrientation == Horizontal) {
        axis = qobject_cast<QValueAxis *>(chart()->axes(Qt::Horizontal).first());
    } else {
        axis = qobject_cast<QValueAxis *>(chart()->axes(Qt::Vertical).first());
    }

    if (axis) {
        axis->setRange(minValue, maxValue);
        // qDebug() << "Axis range updated to:" << axis->min() << "-" << axis->max();
    } else {
        qDebug() << "Axis not found for orientation:" << currentOrientation;
    }
}

void BarPlot::setupAxes(Orientation orientation) {
    qreal minValue, maxValue;
    calculateDynamicRange(minValue, maxValue);

    if (orientation == Horizontal) {
        QValueAxis *axisX = new QValueAxis();
        axisX->setTitleText("Gradients");
        axisX->setLabelFormat("%.2f");
        axisX->setTickCount(5);
        axisX->setRange(minValue, maxValue);
        axisX->setGridLineVisible(false);
        chart()->addAxis(axisX, Qt::AlignBottom);
        barSeries->attachAxis(axisX);

        QCategoryAxis *axisY = new QCategoryAxis();
        for (int i = 0; i < labels.size(); ++i) {
            axisY->append(labels[i], i);
        }
        axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
        axisY->setGridLineVisible(false);
        chart()->addAxis(axisY, Qt::AlignLeft);
        barSeries->attachAxis(axisY);
    } else {
        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("Gradients");
        axisY->setLabelFormat("%.2f");
        axisY->setTickCount(5);
        axisY->setRange(minValue, maxValue);
        axisY->setGridLineVisible(false);
        chart()->addAxis(axisY, Qt::AlignLeft);
        barSeries->attachAxis(axisY);

        QCategoryAxis *axisX = new QCategoryAxis();
        for (int i = 0; i < labels.size(); ++i) {
            axisX->append(labels[i], i);
        }
        axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
        axisX->setGridLineVisible(false);
        chart()->addAxis(axisX, Qt::AlignBottom);
        barSeries->attachAxis(axisX);
    }
}

void BarPlot::calculateDynamicRange(qreal &minValue, qreal &maxValue) const {
    minValue = std::numeric_limits<qreal>::max(); // Initialize minValue to the maximum possible value
    maxValue = std::numeric_limits<qreal>::min(); // Initialize maxValue to the minimum possible value

    if (!barSeries || barSeries->barSets().isEmpty()) {
        qDebug() << "Bar series is empty in calculateDynamicRange";
        minValue = 0; // Fallback to zero if no data is available
        maxValue = 0;
        return;
    }

    const auto &barSets = barSeries->barSets();
    for (const auto *set : barSets) {
        for (int i = 0; i < set->count(); ++i) {
            qreal value = set->at(i);
            if (value < minValue) {
                minValue = value;
            }
            if (value > maxValue) {
                maxValue = value;
            }
        }
    }

    // If all values are zero or no data is available, set a default range
    if (minValue == std::numeric_limits<qreal>::max() && maxValue == std::numeric_limits<qreal>::min()) {
        minValue = 0;
        maxValue = 0;
    }

    // qDebug() << "Dynamic range calculated: minValue =" << minValue << ", maxValue =" << maxValue;
}

void BarPlot::cleanupOldSeries() {
    if (barSeries && chart()->series().contains(barSeries)) {
        chart()->removeSeries(barSeries);
        delete barSeries;
        barSeries = nullptr;
    }
}

void BarPlot::cleanupOldAxes() {
    QList<QAbstractAxis *> axes = chart()->axes();
    for (QAbstractAxis *axis : std::as_const(axes)) {
        chart()->removeAxis(axis);
        delete axis;
    }
}

void BarPlot::startHighlightSelection(const QColor &color, const QList<int>& indices) {
    if (!barSeries) return;

    const auto &barSets = barSeries->barSets();
    for (QBarSet *set : barSets) {
        // Store the original color if it hasn't been stored yet
        if (!originalBarSetColors.contains(set)) {
            originalBarSetColors[set] = set->color();
        }

        if (set->color() == color) {
            // Highlight the bar set by changing its pen and brush
            QPen highlightPen(Qt::yellow, 3); // Example: Yellow border with a width of 3
            set->setPen(highlightPen);

            QBrush highlightBrush(set->color().lighter(150)); // Example: Lighter color for the brush
            set->setBrush(highlightBrush);
        } else {
            // Dim or de-emphasize other bar sets
            QPen defaultPen(Qt::black, 1); // Example: Default black border with a width of 1
            set->setPen(defaultPen);

            QBrush defaultBrush(set->color().darker(150)); // Example: Darker color for the brush
            set->setBrush(defaultBrush);
        }
    }

    // Update the chart to reflect the changes
    chart()->update();
}

void BarPlot::stopHighlightSelection() {
    if (!barSeries) return;

    const auto &barSets = barSeries->barSets();
    for (QBarSet *set : barSets) {
        // Restore the original color if it was stored
        if (originalBarSetColors.contains(set)) {
            QColor originalColor = originalBarSetColors[set];
            set->setBrush(originalColor); // Restore the original brush color
        }

        // Reset the pen to its default values
        QPen defaultPen(Qt::black, 1); // Default black border with a width of 1
        set->setPen(defaultPen);
    }

    // Clear the stored original colors (optional, depending on your use case)
    originalBarSetColors.clear();

    // Update the chart to reflect the changes
    chart()->update();
}

