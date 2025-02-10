#ifndef EXPLANATIONPLOT_H
#define EXPLANATIONPLOT_H

#include <QObject>
#include <QWidget>
#include <QtCharts/QChartView>

class ExplanationPlot: public QChartView
{
    Q_OBJECT
public:
    ExplanationPlot();
};

#endif // EXPLANATIONPLOT_H
