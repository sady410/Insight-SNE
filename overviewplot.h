#ifndef OVERVIEWPLOT_H
#define OVERVIEWPLOT_H

#include <QtCharts/QChartView>
#include <QScatterSeries>

class OverviewPlot : public QChartView {
    Q_OBJECT
public:
    explicit OverviewPlot(QChart *chart, QWidget *parent = nullptr);

    void setScatterSeries(QScatterSeries *series);

private:
    QScatterSeries *scatterSeries;

};

#endif // OVERVIEWPLOT_H
