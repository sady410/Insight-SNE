#ifndef BARPLOT_H
#define BARPLOT_H

#include <QChartView>
#include <QChart>
#include <QAbstractBarSeries>
#include <QBarSet>
#include <QStringList>
#include <QList>
#include <QColor>
#include <QMap>
#include "datamanager.h"

class BarPlot : public QChartView
{
    Q_OBJECT

public:
    explicit BarPlot(QChart *chart, QWidget *parent = nullptr);
    ~BarPlot();

    // Set the data manager
    void setDataManager(DataManagerPtr dataManager);

    // Initialize the plot with orientation
    void initPlot(Qt::Orientation orientation);

    // Add/remove data series
    void addValue(int id, const QColor &color, const QList<int> &indices, const bool enableDelete, const bool state);
    void removeValue(int id);

    // Highlighting methods
    void startHighlightSelection(const QColor &color, const QList<int>& indices);
    void stopHighlightSelection();

signals:
    void barClicked(const QString &label, int id);

private slots:
    void handleBarClicked(int index, QBarSet *barSet);

private:
    void setupAxes(Qt::Orientation orientation, const QStringList& labels);
    void updateAxisRange();
    void calculateDynamicRange(qreal &minValue, qreal &maxValue) const;

    // Data manager
    DataManagerPtr m_dataManager;

    // Plot components
    QAbstractBarSeries *m_barSeries;
    Qt::Orientation m_plotOrientation;

    // Data tracking
    QMap<int, QBarSet*> m_idToBarSetMap;
    QMap<int, QColor> m_initialBarSetColors;
    QMap<QBarSet*, QColor> m_originalBarSetColors;

    // State
    bool m_firstDataAdded;
};

#endif // BARPLOT_H
