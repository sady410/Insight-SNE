#ifndef DISTRIBUTIONPLOT_H
#define DISTRIBUTIONPLOT_H

#include <QChartView>
#include <QMouseEvent>
#include <QMap>
#include <QColor>
#include <QList>
#include <QLineSeries>
#include <QBarSeries>

class DistributionPlot : public QChartView {
    Q_OBJECT

public:
    enum class BinMethod {
        DiscreteValues,
        EqualWidth,
        AutoBin
    };

    explicit DistributionPlot(QChart *chart, QWidget *parent = nullptr);
    ~DistributionPlot();

    void setData(const QList<QList<qreal>> &data);
    void setFeatureNames(const QStringList &featureNames);
    void setCurrentFeature(const QString &featureName);
    void setCurrentSelection(int selectionId);
    void addSelection(int id, const QColor &color, const QList<int> &indices,
                      bool enableDelete = true, bool state = true);
    void removeSelection(int id);
    void setBinMethod(BinMethod method);
    void setMaxBins(int maxBins);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void setupAxes(const QStringList &categories = QStringList());
    void updateAxisRange();
    void calculateDynamicRange(qreal &minValue, qreal &maxValue) const;
    void updatePlot();
    void clearChart();
    QList<qreal> getSelectedValues(int featureIndex) const;
    void createHistogram(const QList<qreal> &values, BinMethod binMethod);
    void createTrendLine();

private:
    QBarSeries *m_barSeries;
    QLineSeries *m_lineSeries;
    QStringList m_featureNames;
    QString m_currentFeatureName;
    QList<QList<qreal>> m_data;
    QMap<int, QColor> m_selectionColors;
    QMap<int, QList<int>> m_selectionIndices;
    int m_currentSelectionId;
    qreal m_minValue;
    qreal m_maxValue;
    BinMethod m_binMethod;
    int m_maxBins;
    QStringList m_barLabels;
    QGraphicsSimpleTextItem *m_statsTextItem;
};

#endif // DISTRIBUTIONPLOT_H
