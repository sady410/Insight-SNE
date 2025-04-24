#include "distributionplot.h"
#include <QBarSet>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QBarSeries>
#include <QDebug>
#include <QtMath>
#include <algorithm>

DistributionPlot::DistributionPlot(QChart *chart, QWidget *parent)
    : QChartView(chart, parent),
    m_barSeries(nullptr),
    m_firstDataAdded(false),
    m_currentFeatureIndex(0),
    m_binCount(10),
    m_minValue(0),
    m_maxValue(0) {

    setRenderHint(QPainter::Antialiasing);
    chart->setTheme(QChart::ChartThemeDark);
}

DistributionPlot::~DistributionPlot() {}

void DistributionPlot::setData(const QList<QList<qreal>> &data) {
    m_data = data;

    // Reset plot when data changes
    if (m_firstDataAdded && m_barSeries) {
        m_barSeries->clear();
        chart()->removeAllSeries();
        m_firstDataAdded = false;
        m_idToBarSetMap.clear();
        m_initialBarSetColors.clear();
        m_originalBarSetColors.clear();
    }
}

void DistributionPlot::setFeatureNames(const QStringList &featureNames) {
    m_featureNames = featureNames;
}

void DistributionPlot::setBinCount(int binCount) {
    m_binCount = binCount > 0 ? binCount : 10; // Ensure at least 1 bin
}

void DistributionPlot::initPlot() {
    m_barSeries = new QBarSeries();
}

void DistributionPlot::setCurrentFeature(int featureIndex) {
    if (featureIndex >= 0 && featureIndex < m_featureNames.size()) {
        m_currentFeatureIndex = featureIndex;

        // Update all existing selections with the new feature
        QList<int> ids = m_idToBarSetMap.keys();
        QMap<int, QList<int>> selectionIndices;
        QMap<int, QColor> selectionColors;

        // Store current selections
        for (int id : ids) {
            // We need to find the indices for each id, which isn't stored directly
            // For a real implementation, you might want to store this in a member variable
            // This is a placeholder
            selectionIndices[id] = QList<int>(); // You'd need to store and retrieve these
            selectionColors[id] = m_initialBarSetColors[id];
        }

        // Clear current plot
        if (m_barSeries) {
            m_barSeries->clear();
            chart()->removeAllSeries();
            m_firstDataAdded = false;
        }

        // Reinitialize the plot
        initPlot();

        // Re-add all selections with the new feature
        for (int id : ids) {
            addSelection(id, selectionColors[id], selectionIndices[id]);
        }
    }
}

void DistributionPlot::addSelection(int id, const QColor &color, const QList<int> &indices,
                                    const bool enableDelete, const bool state) {
    Q_UNUSED(enableDelete);

    if (!state) {
        qDebug() << "State disabled";
        return;
    }
    if (indices.isEmpty()) {
        qDebug() << "No indices provided";
        return;
    }
    if (m_data.isEmpty()) {
        qDebug() << "Data is empty";
        return;
    }

    // Calculate distribution for the current feature
    QList<qreal> binValues;
    QStringList binLabels;
    computeDistribution(m_currentFeatureIndex, indices, binValues, binLabels);

    // Create a new bar set for this selection
    QBarSet *newSet = new QBarSet(QString("Selection %1").arg(id));
    for (qreal value : binValues) {
        *newSet << value;
    }
    newSet->setColor(color);

    // Store the initial color and map the id to the bar set
    m_initialBarSetColors[id] = color;
    m_idToBarSetMap[id] = newSet;

    // Connect click handler
    connect(newSet, &QBarSet::clicked, this, [this, newSet](int index) {
        handleBarClicked(index, newSet);
    });

    // Add to series and update chart
    if (m_barSeries) {
        m_barSeries->append(newSet);
        if (!m_firstDataAdded) {
            chart()->addSeries(m_barSeries);
            m_binLabels = binLabels; // Store bin labels for axis setup
            setupAxes();
            m_firstDataAdded = true;
        }
        updateAxisRange();
    } else {
        delete newSet;
    }
}

void DistributionPlot::removeSelection(int id) {
    if (!m_barSeries || !m_idToBarSetMap.contains(id)) {
        qDebug() << "Invalid ID or series is empty.";
        return;
    }

    QBarSet *barSet = m_idToBarSetMap.value(id);
    if (!barSet) {
        qDebug() << "Bar set not found for ID:" << id;
        return;
    }

    m_barSeries->remove(barSet);
    m_idToBarSetMap.remove(id);
    m_originalBarSetColors.remove(barSet);
    updateAxisRange();
}

void DistributionPlot::computeDistribution(int featureIndex, const QList<int> &indices,
                                           QList<qreal> &binValues, QStringList &binLabels) {
    // Check if feature index is valid
    if (featureIndex < 0 || m_data.isEmpty() || featureIndex >= m_data.first().size()) {
        qDebug() << "Invalid feature index or empty data";
        return;
    }

    // Collect all values for the specified feature from selected indices
    QList<qreal> featureValues;
    for (int index : indices) {
        if (index >= 0 && index < m_data.size()) {
            featureValues.append(m_data[index][featureIndex]);
        }
    }

    if (featureValues.isEmpty()) {
        qDebug() << "No values found for the selected feature and indices";
        return;
    }

    // Find min and max for binning
    qreal minVal = *std::min_element(featureValues.begin(), featureValues.end());
    qreal maxVal = *std::max_element(featureValues.begin(), featureValues.end());

    // Add a small padding to avoid edge cases
    qreal padding = (maxVal - minVal) * 0.01;
    if (padding == 0) padding = 0.1; // In case min equals max

    minVal -= padding;
    maxVal += padding;

    // Store for later use
    m_minValue = minVal;
    m_maxValue = maxVal;

    // Create bins
    qreal binWidth = (maxVal - minVal) / m_binCount;

    // Initialize bin values and labels
    binValues.clear();
    binLabels.clear();
    binValues.fill(0, m_binCount);

    // Create bin labels
    for (int i = 0; i < m_binCount; i++) {
        qreal lowerBound = minVal + i * binWidth;
        qreal upperBound = lowerBound + binWidth;
        binLabels.append(QString("[%1, %2]").arg(lowerBound, 0, 'f', 2).arg(upperBound, 0, 'f', 2));
    }

    // Count values in each bin
    for (qreal value : featureValues) {
        int binIndex = qFloor((value - minVal) / binWidth);
        // Handle edge case for max value
        if (binIndex == m_binCount) binIndex = m_binCount - 1;
        // Handle out of range values (shouldn't happen with padding)
        if (binIndex >= 0 && binIndex < m_binCount) {
            binValues[binIndex]++;
        }
    }
}

void DistributionPlot::handleBarClicked(int index, QBarSet *barSet) {
    if (index < 0 || index >= m_binLabels.size()) {
        qDebug() << "Invalid index:" << index;
        return;
    }

    QString binLabel = m_binLabels[index];

    // Find the id associated with the clicked bar set
    int id = -1;
    for (auto it = m_idToBarSetMap.begin(); it != m_idToBarSetMap.end(); ++it) {
        if (it.value() == barSet) {
            id = it.key();
            break;
        }
    }

    if (id == -1) {
        qDebug() << "Bar set not found in idToBarSetMap";
        return;
    }

    emit barClicked(binLabel, id);
}

void DistributionPlot::setupAxes() {
    // Value axis for Y (count)
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Count");
    axisY->setLabelFormat("%d");
    axisY->setTickCount(5);
    axisY->setGridLineVisible(false);
    chart()->addAxis(axisY, Qt::AlignLeft);
    m_barSeries->attachAxis(axisY);

    // Category axis for X (bin ranges)
    QCategoryAxis *axisX = new QCategoryAxis();
    for (int i = 0; i < m_binLabels.size(); ++i) {
        axisX->append(m_binLabels[i], i);
    }
    axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    axisX->setGridLineVisible(false);
    axisX->setTitleText(m_featureNames.value(m_currentFeatureIndex, "Feature"));
    chart()->addAxis(axisX, Qt::AlignBottom);
    m_barSeries->attachAxis(axisX);
}

void DistributionPlot::updateAxisRange() {
    if (!m_barSeries) {
        qDebug() << "Bar series is null in updateAxisRange";
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
    if (padding == 0) padding = 1; // At least 1 for counts
    maxValue += padding;

    // Get the Y-axis (count axis) for vertical orientation
    QValueAxis *axis = qobject_cast<QValueAxis *>(chart()->axes(Qt::Vertical).first());

    if (axis) {
        axis->setRange(minValue, maxValue);
    } else {
        qDebug() << "Y-axis not found";
    }
}

void DistributionPlot::calculateDynamicRange(qreal &minValue, qreal &maxValue) const {
    minValue = 0; // For count, min is always 0
    maxValue = 0;

    if (!m_barSeries || m_barSeries->barSets().isEmpty()) {
        qDebug() << "Bar series is empty in calculateDynamicRange";
        return;
    }

    const auto &barSets = m_barSeries->barSets();
    for (const auto *set : barSets) {
        for (int i = 0; i < set->count(); ++i) {
            qreal value = set->at(i);
            if (value > maxValue) {
                maxValue = value;
            }
        }
    }

    // If no data is available or all values are zero, set a default range
    if (maxValue == 0) {
        maxValue = 1; // Default to 1 for empty distributions
    }
}
