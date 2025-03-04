#ifndef MAGNITUDESPLOT_H
#define MAGNITUDESPLOT_H

#include <QWidget>
#include <QHorizontalBarSeries>
#include <QtCharts/QChartView>
#include <QScatterSeries>

class MagnitudesPlot : public QChartView {
    Q_OBJECT

public:
    explicit MagnitudesPlot(QChart *chart, QWidget *parent = nullptr);

    void initPlot(QHorizontalBarSeries *horizontalBarSeries);
    void setFeatureNames(const QStringList featureNames);
    void setMagnitudes(const QList<QList<qreal>> magnitudes);

public slots:
    void updateSelectedPoints(const QList<int> &indices, const QColor &color);

private:
    QHorizontalBarSeries *horizontalBarSeries;
    QStringList featureNames; // TODO pointer ?
    QList<QList<qreal>> magnitudes; // TODO pointer ?

    void addNewMagnitude(const QList<int> &indices, const QColor &color);
};

#endif // MAGNITUDESPLOT_H
