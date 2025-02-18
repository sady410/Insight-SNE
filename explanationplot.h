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
    void setFeatureNames(const QStringList &featureNames);

private:
    QHorizontalBarSeries *horizontalBarSeries;
    QStringList featureNames;
};

#endif // EXPLANATIONPLOT_H
