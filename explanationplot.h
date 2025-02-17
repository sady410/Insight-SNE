#ifndef EXPLANATIONPLOT_H
#define EXPLANATIONPLOT_H

#include <QWidget>
#include <QHorizontalBarSeries>
#include <QtCharts/QChartView>

class ExplanationPlot : public QChartView {
    Q_OBJECT

public:
     explicit ExplanationPlot(QChart *chart, QHorizontalBarSeries *series, QWidget *parent = nullptr);

private:
    QHorizontalBarSeries *horizontalBarSeries;
};

#endif // EXPLANATIONPLOT_H
