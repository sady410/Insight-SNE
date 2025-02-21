#ifndef OVERVIEWPLOT_H
#define OVERVIEWPLOT_H

#include <QtCharts/QChartView>
#include <QScatterSeries>

class OverviewPlot : public QChartView {
    Q_OBJECT
public:
    explicit OverviewPlot(QChart *chart, QWidget *parent = nullptr);

    void setScatterSeries(QScatterSeries *series);

public slots:
    void updateZoomRectangle(qreal minX, qreal maxX, qreal minY, qreal maxY);

private:
    QScatterSeries *scatterSeries;
    QGraphicsRectItem *zoomRectItem;
};

#endif // OVERVIEWPLOT_H
