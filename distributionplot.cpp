#include "distributionplot.h"
#include <QBarSet>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QBarSeries>
#include <QDebug>
#include <QtMath>
#include <QBarCategoryAxis>
#include <algorithm>
#include <QLineSeries>
#include <QScatterSeries>
#include <QPainter>
#include <QToolTip>

DistributionPlot::DistributionPlot(QChart *chart, QWidget *parent)
    : QChartView(chart, parent),
    m_barSeries(nullptr),
    m_lineSeries(nullptr),
    m_currentSelectionId(-1),
    m_minValue(0),
    m_maxValue(0),
    m_binMethod(BinMethod::AutoBin),
    m_maxBins(20),
    m_statsTextItem(nullptr) {

    setRenderHint(QPainter::Antialiasing);
    chart->legend()->setVisible(false);

    // Enable mouse tracking for tooltips
    setMouseTracking(true);
}

DistributionPlot::~DistributionPlot() {
    // Make sure we fully clean up
    clearChart();

    // Additional check for any remaining text items
    if (m_statsTextItem) {
        delete m_statsTextItem;
        m_statsTextItem = nullptr;
    }
}

void DistributionPlot::clearChart() {
    chart()->removeAllSeries();

    // Remove all axes
    const auto axes = chart()->axes();
    for (auto *axis : axes) {
        chart()->removeAxis(axis);
        delete axis;
    }

    // Clean up stats text item
    if (m_statsTextItem) {
        // Remove from scene if it's in the scene
        if (m_statsTextItem->scene()) {
            m_statsTextItem->scene()->removeItem(m_statsTextItem);
        }
        delete m_statsTextItem;
        m_statsTextItem = nullptr;
    }

    m_barSeries = nullptr;
    m_lineSeries = nullptr;
}

void DistributionPlot::setData(const QList<QList<qreal>> &data) {
    m_data = data;
}

void DistributionPlot::setFeatureNames(const QStringList &featureNames) {
    m_featureNames = featureNames;
}

void DistributionPlot::setCurrentFeature(const QString &featureName) {
    if (m_currentFeatureName != featureName) {
        m_currentFeatureName = featureName;
        updatePlot();
    }
}

void DistributionPlot::setCurrentSelection(int selectionId) {
    if (m_currentSelectionId != selectionId) {
        m_currentSelectionId = selectionId;
        updatePlot();
    }
}

void DistributionPlot::addSelection(int id, const QColor &color, const QList<int> &indices, bool enableDelete, bool state) {
    Q_UNUSED(enableDelete);
    Q_UNUSED(state);

    m_selectionColors[id] = color;
    m_selectionIndices[id] = indices;

    // If this is the first selection, automatically set it as current
    if (m_currentSelectionId == -1) {
        m_currentSelectionId = id;
        updatePlot();
    }
}

void DistributionPlot::removeSelection(int id) {
    // Remove the selection from our maps
    m_selectionColors.remove(id);
    m_selectionIndices.remove(id);

    // If the removed selection was the current one, reset current selection
    if (m_currentSelectionId == id) {
        // Try to set the first available selection as current
        m_currentSelectionId = m_selectionIndices.isEmpty() ? -1 : m_selectionIndices.keys().first();
        updatePlot();
    }
}

void DistributionPlot::setBinMethod(BinMethod method) {
    if (m_binMethod != method) {
        m_binMethod = method;
        updatePlot();
    }
}

void DistributionPlot::setMaxBins(int maxBins) {
    if (m_maxBins != maxBins && maxBins > 0) {
        m_maxBins = maxBins;
        if (m_binMethod != BinMethod::DiscreteValues) {
            updatePlot();
        }
    }
}

void DistributionPlot::setupAxes(const QStringList &categories) {
    // Value axis for Y (count)
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Count");
    axisY->setLabelFormat("%d");
    axisY->setTickCount(5);
    axisY->setGridLineVisible(true);
    axisY->setGridLineColor(QColor(60, 60, 60));
    chart()->addAxis(axisY, Qt::AlignLeft);

    if (m_barSeries) {
        m_barSeries->attachAxis(axisY);
    }

    if (m_lineSeries) {
        m_lineSeries->attachAxis(axisY);
    }

    // Category axis for X (bin ranges)
    if (categories.isEmpty()) {
        QValueAxis *axisX = new QValueAxis();
        axisX->setGridLineVisible(true);
        axisX->setGridLineColor(QColor(60, 60, 60));
        axisX->setTitleText(m_currentFeatureName);
        chart()->addAxis(axisX, Qt::AlignBottom);

        if (m_barSeries) {
            m_barSeries->attachAxis(axisX);
        }

        if (m_lineSeries) {
            m_lineSeries->attachAxis(axisX);
        }
    } else {
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        axisX->setTitleText(m_currentFeatureName);
        axisX->setGridLineVisible(true);
        axisX->setGridLineColor(QColor(60, 60, 60));

        // Handle label visibility based on number of categories
        if (categories.size() > 10) {
            axisX->setLabelsAngle(45);
            // Show only some labels to prevent overcrowding
            int step = qMax(1, categories.size() / 10);
            for (int i = 0; i < categories.size(); ++i) {
                axisX->setLabelsVisible(i % step == 0);
            }
        }

        chart()->addAxis(axisX, Qt::AlignBottom);

        if (m_barSeries) {
            m_barSeries->attachAxis(axisX);
        }

        if (m_lineSeries) {
            m_lineSeries->attachAxis(axisX);
        }
    }
}

void DistributionPlot::updateAxisRange() {
    if (!m_barSeries) {
        return;
    }

    qreal minValue = 0; // For count, min is always 0
    qreal maxValue = 0;

    const auto &barSets = m_barSeries->barSets();
    for (const auto *set : barSets) {
        for (int i = 0; i < set->count(); ++i) {
            qreal value = set->at(i);
            if (value > maxValue) {
                maxValue = value;
            }
        }
    }

    // Add padding to max value
    qreal padding = maxValue * 0.1;
    if (padding < 1) padding = 1; // At least 1 for counts
    maxValue += padding;

    // Update the Y-axis (count axis) for vertical orientation
    auto axes = chart()->axes(Qt::Vertical);
    if (!axes.isEmpty()) {
        QValueAxis *axis = qobject_cast<QValueAxis *>(axes.first());
        if (axis) {
            axis->setRange(minValue, maxValue);
        }
    }
}

void DistributionPlot::mouseMoveEvent(QMouseEvent *event) {
    QChartView::mouseMoveEvent(event);

    QPoint pos = event->pos();
    QPointF scenePos = mapToScene(pos);
    QPointF chartPos = chart()->mapFromScene(scenePos);
    QPointF plotPos = chart()->mapToValue(chartPos);

    // Only show tooltip if we have data and barSeries
    if (m_barSeries && !m_barSeries->barSets().isEmpty()) {
        // Find the bar under the cursor
        int barIndex = qFloor(plotPos.x());

        if (barIndex >= 0 && barIndex < m_barLabels.size()) {
            QString label = m_barLabels[barIndex];
            int count = m_barSeries->barSets().first()->at(barIndex);
            QString tooltip = QString("%1\nCount: %2").arg(label).arg(count);
            QToolTip::showText(event->globalPosition().toPoint(), tooltip, this);
            return;
        }
    }

    QToolTip::hideText();
}

QList<qreal> DistributionPlot::getSelectedValues(int featureIndex) const {
    QList<qreal> selectedValues;

    if (m_currentSelectionId != -1 && m_selectionIndices.contains(m_currentSelectionId)) {
        const QList<int> &indices = m_selectionIndices[m_currentSelectionId];

        for (int idx : indices) {
            if (idx >= 0 && idx < m_data.size() && featureIndex < m_data[idx].size()) {
                selectedValues.append(m_data[idx][featureIndex]);
            }
        }
    }

    return selectedValues;
}

void DistributionPlot::createHistogram(const QList<qreal> &values, BinMethod binMethod) {
    // Clear previous data
    m_barLabels.clear();

    if (values.isEmpty()) {
        chart()->setTitle(QString("No data for %1").arg(m_currentFeatureName));
        return;
    }

    // Find min/max values
    qreal minVal = *std::min_element(values.begin(), values.end());
    qreal maxVal = *std::max_element(values.begin(), values.end());

    // Create a bar set for the histogram
    QBarSet *histogramSet = new QBarSet("");

    // Use the color for this selection if available
    QColor barColor = Qt::red; // Default color
    if (m_selectionColors.contains(m_currentSelectionId)) {
        barColor = m_selectionColors[m_currentSelectionId];
    }
    histogramSet->setColor(barColor);

    // Create the histogram based on binning method
    switch (binMethod) {
    case BinMethod::DiscreteValues: {
        // Count occurrences of each unique value
        QMap<qreal, int> valueCounts;
        for (const qreal &value : values) {
            valueCounts[value]++;
        }

        // Sort the unique values
        QList<qreal> uniqueValues = valueCounts.keys();
        std::sort(uniqueValues.begin(), uniqueValues.end());

        // Add count for each unique value
        for (const qreal &value : uniqueValues) {
            *histogramSet << valueCounts[value];

            // Format the value label
            QString label;
            if (qAbs(value - qFloor(value)) < 0.00001) {
                label = QString::number(static_cast<int>(value));
            } else {
                label = QString::number(value, 'f', 2);
            }
            m_barLabels.append(label);
        }

        chart()->setTitle(QString("Distribution of %1 (%2 unique values)")
                              .arg(m_currentFeatureName)
                              .arg(uniqueValues.size()));
        break;
    }

    case BinMethod::EqualWidth: {
        // Equal width bins
        int numBins = qMin(m_maxBins, static_cast<int>(values.size() / 5 + 1));
        if (numBins < 1) numBins = 1;

        // Calculate bin width
        qreal range = maxVal - minVal;
        if (range <= 0) {
            // Handle the case where all values are the same
            numBins = 1;
            range = 1; // Prevent division by zero
        }

        qreal binWidth = range / numBins;

        // Initialize bin counts
        QVector<int> binCounts(numBins, 0);

        // Assign values to bins
        for (const qreal &value : values) {
            int binIndex = qMin(numBins - 1,
                                static_cast<int>((value - minVal) / binWidth));
            binCounts[binIndex]++;
        }

        // Create the histogram
        for (int i = 0; i < numBins; ++i) {
            *histogramSet << binCounts[i];

            // Create bin labels
            qreal binStart = minVal + i * binWidth;
            qreal binEnd = i < numBins - 1 ? minVal + (i + 1) * binWidth : maxVal;

            QString startStr = QString::number(binStart, 'f', 2);
            QString endStr = QString::number(binEnd, 'f', 2);
            m_barLabels.append(QString("[%1, %2]").arg(startStr).arg(endStr));
        }

        chart()->setTitle(QString("Distribution of %1 (%2 bins)")
                              .arg(m_currentFeatureName)
                              .arg(numBins));
        break;
    }

    case BinMethod::AutoBin: {
        // Use Sturges' formula to determine number of bins: k = ⌈log₂(n) + 1⌉
        int n = values.size();
        int numBins = qMin(m_maxBins, static_cast<int>(qCeil(qLn(n) / qLn(2) + 1)));

        if (numBins < 1) numBins = 1;

        // Now use equal width bins
        qreal range = maxVal - minVal;
        if (range <= 0) {
            numBins = 1;
            range = 1;
        }

        qreal binWidth = range / numBins;

        // Initialize bin counts
        QVector<int> binCounts(numBins, 0);

        // Assign values to bins
        for (const qreal &value : values) {
            int binIndex = qMin(numBins - 1,
                                static_cast<int>((value - minVal) / binWidth));
            binCounts[binIndex]++;
        }

        // Create the histogram
        for (int i = 0; i < numBins; ++i) {
            *histogramSet << binCounts[i];

            // Create bin labels
            qreal binStart = minVal + i * binWidth;
            qreal binEnd = i < numBins - 1 ? minVal + (i + 1) * binWidth : maxVal;

            QString startStr = QString::number(binStart, 'f', 2);
            QString endStr = QString::number(binEnd, 'f', 2);
            m_barLabels.append(QString("[%1, %2]").arg(startStr).arg(endStr));
        }

        chart()->setTitle(QString("Distribution of %1 (Auto-binned: %2 bins)")
                              .arg(m_currentFeatureName)
                              .arg(numBins));
        break;
    }
    }

    m_barSeries->append(histogramSet);
}

void DistributionPlot::createTrendLine() {
    if (!m_barSeries || m_barSeries->barSets().isEmpty()) {
        return;
    }

    // Create a line series to overlay on the histogram
    m_lineSeries = new QLineSeries();

    // Set line appearance - slightly darker than the bar color for contrast
    QColor barColor = m_barSeries->barSets().first()->color();
    QColor lineColor = barColor.darker(120);
    m_lineSeries->setColor(lineColor);
    m_lineSeries->setPen(QPen(lineColor, 2, Qt::SolidLine));

    // Add points to the line series - one point for each bar
    const QBarSet *barSet = m_barSeries->barSets().first();

    // Add an initial point at the start of the first bin
    if (barSet->count() > 0) {
        m_lineSeries->append(0.0, barSet->at(0));
    }

    // Add points at the center of each bar
    for (int i = 0; i < barSet->count(); ++i) {
        m_lineSeries->append(i + 0.5, barSet->at(i));
    }

    // Add a final point at the end of the last bin
    if (barSet->count() > 0) {
        m_lineSeries->append(barSet->count(), barSet->at(barSet->count() - 1));
    }

    // Add the line series to the chart
    chart()->addSeries(m_lineSeries);
}

void DistributionPlot::updatePlot() {
    // Clear previous chart FIRST to ensure no text items remain
    clearChart();

    // Create new bar series
    m_barSeries = new QBarSeries();
    chart()->addSeries(m_barSeries);

    // Find the feature index
    int featureIndex = m_featureNames.indexOf(m_currentFeatureName);
    if (featureIndex == -1 || m_data.isEmpty() || featureIndex >= m_data.first().size()) {
        setupAxes(QStringList());
        chart()->setTitle(QString("Invalid feature: %1").arg(m_currentFeatureName));
        return;
    }

    // Get selected values for this feature
    QList<qreal> selectedValues = getSelectedValues(featureIndex);

    if (selectedValues.isEmpty()) {
        setupAxes(QStringList());
        chart()->setTitle(QString("No selection for %1").arg(m_currentFeatureName));
        return;
    }

    // Create the histogram using the appropriate binning method
    createHistogram(selectedValues, m_binMethod);

    // Add trend line
    createTrendLine();

    // Setup axes with labels
    setupAxes(m_barLabels);

    // Update axis ranges
    updateAxisRange();

    // Add statistics
    if (!selectedValues.isEmpty()) {
        // Calculate basic statistics
        qreal mean = 0;
        for (const qreal &value : selectedValues) {
            mean += value;
        }
        mean /= selectedValues.size();

        qreal variance = 0;
        for (const qreal &value : selectedValues) {
            variance += (value - mean) * (value - mean);
        }
        variance /= selectedValues.size();
        qreal stdDev = qSqrt(variance);

        // Add statistics as a text item
        QString statsText = QString("n=%1, μ=%2, σ=%3")
                                .arg(selectedValues.size())
                                .arg(mean, 0, 'f', 2)
                                .arg(stdDev, 0, 'f', 2);

        // Create a new text item
        m_statsTextItem = new QGraphicsSimpleTextItem(chart());
        m_statsTextItem->setText(statsText);
        m_statsTextItem->setBrush(Qt::white);
        m_statsTextItem->setPos(50, chart()->margins().top() + 5);
        m_statsTextItem->setZValue(1000);  // Ensure it's on top of everything
    }
}
