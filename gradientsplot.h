#ifndef GRADIENTSPLOT_H
#define GRADIENTSPLOT_H

#include <QWidget>
#include <QHorizontalBarSeries>
#include <QtCharts/QChartView>
#include <QScatterSeries>

class GradientsPlot : public QChartView {
    Q_OBJECT

public:
    explicit GradientsPlot(QChart *chart, QWidget *parent = nullptr);

    void initPlot(QHorizontalBarSeries *horizontalBarSeries);
    void setFeatureNames(const QStringList featureNames);
    void setGradients(const QList<QList<qreal>> gradients);
    void setMeanGradients(const QList<QList<qreal>> meanGradients);

public slots:
    void updateSelectedPoints(const QList<int> &indices, const QColor &color);

private:
    QHorizontalBarSeries *horizontalBarSeries;
    QStringList featureNames; // TODO pointer ?
    QList<QList<qreal>> gradients;
    QList<QList<qreal>> meanGradients;
    void addNewGradient(const QList<int> &indices, const QColor &color);
};

#endif // MAGNITUDESPLOT_H
