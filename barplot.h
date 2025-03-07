#ifndef BARPLOT_H
#define BARPLOT_H

#include <QWidget>
#include <QAbstractBarSeries>
#include <QtCharts/QChartView>
#include <QScatterSeries>

class BarPlot : public QChartView {
    Q_OBJECT

public:
    enum Orientation {
        Horizontal,
        Vertical
    };

    explicit BarPlot(QChart *chart, QWidget *parent = nullptr);

    void initPlot(QAbstractBarSeries *series, Orientation orientation);
    void setOrientation(Orientation orientation);
    void setLabels(const QStringList labels);
    void setValues(const QList<QList<qreal>> gradients);
    void setMeanValues(const QList<QList<qreal>> meanGradients);

public slots:
    void addValue(const QList<int> &indices, const QColor &color);
    void removeValueByLabel(const QString &label); // TODO not used and maybe only need removeValue

private:
    Orientation currentOrientation;
    QAbstractBarSeries *barSeries;
    QStringList labels; // TODO pointer ?
    QList<QList<qreal>> values;
    QList<QList<qreal>> meanValues;

    void removeValue(int index);
};

#endif // BARPLOT_H
