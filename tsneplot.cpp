#include "TsnePlot.h"

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QPainterPath>
#include <QPen>
#include <QDebug>

TsnePlot::TsnePlot(QChart *chart, QScatterSeries *series, QWidget *parent)
    : QChartView(chart, parent), isDrawing(false), pathItem(nullptr), scatterSeries(series) {
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
}

void TsnePlot::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDrawing = true;
        QPointF scenePos = event->pos();
        path = QPainterPath(scenePos);
        firstPoint = scenePos;

        pathItem = new QGraphicsPathItem();
        pathItem->setPath(path);

        QPen pen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        pathItem->setPen(pen);

        chart()->scene()->addItem(pathItem);
    }
}

void TsnePlot::mouseMoveEvent(QMouseEvent *event) {
    if (isDrawing) {
        QPointF scenePos = event->pos();
        path.lineTo(scenePos);
        pathItem->setPath(path);
    }
}

void TsnePlot::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDrawing = false;
        path.lineTo(firstPoint);
        path.closeSubpath();
        pathItem->setPath(path);
        selectPointsInsideLasso();
    }
}

void TsnePlot::selectPointsInsideLasso() {
    qDebug() << "Checking points inside lasso...";

    for (int i = 0; i < scatterSeries->count(); ++i) {
        QPointF point = scatterSeries->at(i);
        QPointF screenPos = chart()->mapToPosition(point);

        if (path.contains(screenPos)) {
            qDebug() << "Point inside lasso:" << point;
            scatterSeries->replace(i, QPointF(point.x(), point.y() + 2));
        }
    }
}
