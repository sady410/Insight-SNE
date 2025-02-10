#ifndef TSNEPLOT_H
#define TSNEPLOT_H

#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QGraphicsPathItem>
#include <QMouseEvent>
#include <QPainterPath>
#include <QPen>

// QT_FORWARD_DECLARE_CLASS(QChart);

class TsnePlot : public QChartView {
    Q_OBJECT

public:
    explicit TsnePlot(QChart *chart, QScatterSeries *series, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool isDrawing;
    QPainterPath path;
    QGraphicsPathItem *pathItem;
    QScatterSeries *scatterSeries;
    QPointF firstPoint;

    void selectPointsInsideLasso();
};

#endif // TSNEPLOT_H
