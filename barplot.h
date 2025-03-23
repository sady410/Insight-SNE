#ifndef BARPLOT_H
#define BARPLOT_H

#include <QWidget>
#include <QAbstractBarSeries>
#include <QtCharts/QChartView>
#include <QScatterSeries>
#include <QMap>

class BarPlot : public QChartView {
    Q_OBJECT

public:
    explicit BarPlot(QChart *chart, QWidget *parent = nullptr);
    ~BarPlot();

    void initPlot(Qt::Orientation orientation);
    void setLabels(const QStringList &labels);
    void setValues(const QList<QList<qreal>> &values);

public slots:
    void addValue(int id, const QColor &color, const QList<int> &indices, const bool enableDelete, const bool state);
    void removeValue(int id);

    void startHighlightSelection(const QColor &color, const QList<int>& indices);
    void stopHighlightSelection();

    void handleBarClicked(int index, QBarSet *barSet);

signals:
    void barClicked(const QString &label, int id);

private:
    void updateAxisRange();
    void setupAxes(Qt::Orientation orientation);
    void calculateDynamicRange(qreal &minValue, qreal &maxValue) const;

    Qt::Orientation m_plotOrientation;

    QAbstractBarSeries *m_barSeries;
    QMap<int, QBarSet*> m_idToBarSetMap;

    QMap<int, QColor> m_initialBarSetColors;
    QMap<QBarSet*, QColor> m_originalBarSetColors;

    QStringList m_labels;
    QList<QList<qreal>> m_values;

    bool m_firstDataAdded;
};

#endif // BARPLOT_H
