#ifndef OVERVIEWPLOT_H
#define OVERVIEWPLOT_H

#include <QtCharts/QChartView>
#include <QScatterSeries>
#include <QValueAxis>

class OverviewPlot : public QChartView {
    Q_OBJECT
public:
    explicit OverviewPlot(QChart *chart, QWidget *parent = nullptr);

    void setScatterSeries(QScatterSeries *series);

public slots:
    void updateZoomRectangle(qreal minX, qreal maxX, qreal minY, qreal maxY);

private:
    QScatterSeries *m_scatterSeries;
    QGraphicsRectItem *m_zoomRectItem;

    QValueAxis *m_axisX = new QValueAxis();
    QValueAxis *m_axisY = new QValueAxis();

};

#endif // OVERVIEWPLOT_H
