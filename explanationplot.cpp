#include "explanationplot.h"

ExplanationPlot::ExplanationPlot(QChart *chart, QHorizontalBarSeries *series, QWidget *parent)
    : QChartView(chart, parent), horizontalBarSeries(series) {
    setRenderHint(QPainter::Antialiasing);
}
