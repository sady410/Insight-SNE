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
    enum Orientation { // TODO use Qt enum for orientation
        Horizontal,
        Vertical
    };

    explicit BarPlot(QChart *chart, QWidget *parent = nullptr);
    ~BarPlot();

    void initPlot(QAbstractBarSeries *series, Orientation orientation);
    void setOrientation(Orientation orientation);
    void setLabels(const QStringList &labels);
    void setValues(const QList<QList<qreal>> &values);
    void setMeanValues(const QList<QList<qreal>> &meanValues);

public slots:
    void addValue(int id, const QColor &color, const QList<int> &indices);
    void removeValue(int id);

    void startHighlightSelection(const QColor &color, const QList<int>& indices);
    void stopHighlightSelection();

private:
    void updateAxisRange();
    void setupAxes(Orientation orientation);
    void calculateDynamicRange(qreal &minValue, qreal &maxValue) const;
    void cleanupOldSeries();
    void cleanupOldAxes();

    Orientation currentOrientation;

    QAbstractBarSeries *barSeries;
    QMap<int, QBarSet*> idToBarSetMap;

    QMap<int, QColor> initialBarSetColors;
    QMap<QBarSet*, QColor> originalBarSetColors;

    QStringList labels;
    QList<QList<qreal>> values;
    QList<QList<qreal>> meanValues;
};

#endif // BARPLOT_H
