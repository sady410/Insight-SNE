#ifndef EXPLANATIONPLOT_H
#define EXPLANATIONPLOT_H

#include <QWidget>
#include <QHorizontalBarSeries>
#include <QtCharts/QChartView>
#include <QScatterSeries>

class ExplanationPlot : public QChartView {
    Q_OBJECT

public:
    explicit ExplanationPlot(QChart *chart, QWidget *parent = nullptr);

    void setHorizontalBarSeries(QHorizontalBarSeries *horizontalBarSeries);
    void setFeatureNames(const QStringList featureNames);
    void setMagnitudes(const QList<QList<qreal>> magnitudes);
    void setGradients(const QList<QList<QList<qreal>>> gradients);

public slots:
    void updateSelectedPoints(const QList<int> &indices);

private:
    QHorizontalBarSeries *horizontalBarSeries;
    QStringList featureNames; // TODO pointer ?
    QList<QList<qreal>> magnitudes; // TODO pointer ?
    QList<QList<QList<qreal>>> gradients;
};

#endif // EXPLANATIONPLOT_H
