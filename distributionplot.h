#ifndef DISTRIBUTIONPLOT_H
#define DISTRIBUTIONPLOT_H

#include <QChartView>
#include <QList>
#include <QMap>
#include <QColor>
#include <QHash>
#include <QStringList>
#include <QBarSet>
#include <QBarSeries>

class DistributionPlot : public QChartView {
    Q_OBJECT

public:
    DistributionPlot(QChart *chart, QWidget *parent = nullptr);
    ~DistributionPlot();

    // Setters for data
    void setData(const QList<QList<qreal>> &data);
    void setFeatureNames(const QStringList &featureNames);
    void setBinCount(int binCount);

    // Initialize the plot
    void initPlot();

public slots:
    // Add a selection with associated id and color
    void addSelection(int id, const QColor &color, const QList<int> &indices,
                      const bool enableDelete = true, const bool state = true);
    void removeSelection(int id);
    void setCurrentFeature(int featureIndex);

signals:
    void barClicked(const QString &binLabel, int selectionId);

private slots:
    void handleBarClicked(int index, QBarSet *barSet);

private:
    void setupAxes();
    void updateAxisRange();
    void calculateDynamicRange(qreal &minValue, qreal &maxValue) const;
    void computeDistribution(int featureIndex, const QList<int> &indices,
                             QList<qreal> &binValues, QStringList &binLabels);

    QBarSeries *m_barSeries;
    bool m_firstDataAdded;

    // Data
    QList<QList<qreal>> m_data;
    QStringList m_featureNames;
    int m_currentFeatureIndex;
    int m_binCount;

    // Maps to track selections and colors
    QMap<int, QBarSet*> m_idToBarSetMap;
    QMap<int, QColor> m_initialBarSetColors;
    QMap<QBarSet*, QColor> m_originalBarSetColors;

    // Distribution metadata
    qreal m_minValue;
    qreal m_maxValue;
    QStringList m_binLabels;
};

#endif // DISTRIBUTIONPLOT_H
