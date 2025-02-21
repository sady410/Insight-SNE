#ifndef TSNEPLOT_H
#define TSNEPLOT_H

#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QGraphicsPathItem>
#include <QMouseEvent>
#include <QPainterPath>
#include <QPen>

class TsnePlot : public QChartView {
    Q_OBJECT

public:
    explicit TsnePlot(QChart *chart, QWidget *parent = nullptr);

    void setScatterSeries(QScatterSeries *series);
    void setLabels(const QStringList &labels);

signals:
    void zoomChanged(qreal minX, qreal maxX, qreal minY, qreal maxY);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    bool isDrawing;
    QPainterPath path;
    QGraphicsPathItem *pathItem;
    QMap<QString, QScatterSeries *> scatterSeriesMap;
    QPointF firstPoint;
    QStringList labels;
    void updateOverviewRectangle();
    void selectPointsInsideLasso();
};

#endif // TSNEPLOT_H
