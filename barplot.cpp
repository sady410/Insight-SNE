#include "barplot.h"
#include <QBarSet>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QHorizontalBarSeries>
#include <QBarSeries>
#include <QDebug>

BarPlot::BarPlot(QChart *chart, QWidget *parent)
    : QChartView(chart, parent), barSeries(nullptr), firstDataAdded(false) {
    setRenderHint(QPainter::Antialiasing);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTheme(QChart::ChartThemeDark);
}

BarPlot::~BarPlot() {}

void BarPlot::setLabels(const QStringList &labels) {
    this->labels = labels;
}

void BarPlot::setValues(const QList<QList<qreal>> &values) {
    this->values = values;
}

void BarPlot::initPlot(Qt::Orientation orientation) {
    if (orientation == Qt::Horizontal) {
        barSeries = new QHorizontalBarSeries();
    } else {
        barSeries = new QBarSeries();
    }
    plotOrientation = orientation;
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

    connect(newSet, &QBarSet::clicked, this, [this, newSet](int index) {
        handleBarClicked(index, newSet);
    });

    if (barSeries) {
        barSeries->append(newSet);
        if (!firstDataAdded) {
            chart()->addSeries(barSeries);
            setupAxes(plotOrientation);
            firstDataAdded = true;
        }
        updateAxisRange();
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

    axis = qobject_cast<QValueAxis *>(chart()->axes(plotOrientation).first());

    if (axis) {
        axis->setRange(minValue, maxValue);
        // qDebug() << "Axis range updated to:" << axis->min() << "-" << axis->max();
    } else {
        qDebug() << "Axis not found for orientation:" << plotOrientation;
    }
}

void BarPlot::setupAxes(Qt::Orientation orientation) {
    qreal minValue, maxValue;
    calculateDynamicRange(minValue, maxValue);

    if (orientation == Qt::Horizontal) {
        QValueAxis *axisX = new QValueAxis();
        axisX->setTitleText("Gradients"); // TODO
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
    minValue = 0; // Initialize minValue to 0
    maxValue = std::numeric_limits<qreal>::min(); // Initialize maxValue to the minimum possible value

    if (!barSeries || barSeries->barSets().isEmpty()) {
        qDebug() << "Bar series is empty in calculateDynamicRange";
        maxValue = 0; // Fallback to zero if no data is available
        return;
    }

    const auto &barSets = barSeries->barSets();
    for (const auto *set : barSets) {
        for (int i = 0; i < set->count(); ++i) {
            qreal value = set->at(i);
            if (value < minValue) {
                minValue = value; // Update minValue only if a negative value is found
            }
            if (value > maxValue) {
                maxValue = value; // Always update maxValue
            }
        }
    }

    // If no data is available or all values are zero, set a default range
    if (maxValue == std::numeric_limits<qreal>::min()) {
        maxValue = 0;
    }

    // qDebug() << "Dynamic range calculated: minValue =" << minValue << ", maxValue =" << maxValue;
}

void BarPlot::handleBarClicked(int index, QBarSet *barSet) {
    if (index < 0 || index >= labels.size()) {
        qDebug() << "Invalid index:" << index;
        return;
    }

    QString label = labels[index];

    // Find the id associated with the clicked bar set
    int id = -1;
    for (auto it = idToBarSetMap.begin(); it != idToBarSetMap.end(); ++it) {
        if (it.value() == barSet) {
            id = it.key();
            break;
        }
    }

    if (id == -1) {
        qDebug() << "Bar set not found in idToBarSetMap";
        return;
    }

    qDebug() << label << id;
    // Emit the custom signal with the label and id
    emit barClicked(label, id);
}

void BarPlot::startHighlightSelection(const QColor &color, const QList<int>& indices) {
    if (!barSeries) return;

    const auto &barSets = barSeries->barSets();
    for (QBarSet *set : barSets) {
        // Store the original color if it hasn't been stored yet
        if (!originalBarSetColors.contains(set)) {
            originalBarSetColors[set] = set->color();
        }

        if (set->color() != color) {
            QPen defaultPen(Qt::black, 1); // Example: Default black border with a width of 1
            set->setPen(defaultPen);

            QBrush defaultBrush(set->color().darker(150)); // Example: Darker color for the brush
            set->setBrush(defaultBrush);
        }
    }
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
        QPen defaultPen(Qt::white, 1); // Default black border with a width of 1
        set->setPen(defaultPen);
    }

    // Clear the stored original colors
    originalBarSetColors.clear();
    chart()->update();
}

