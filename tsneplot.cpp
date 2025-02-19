#include "tsneplot.h"

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QPainterPath>
#include <QPen>
#include <QDebug>
#include <QValueAxis>

TsnePlot::TsnePlot(QChart *chart, QWidget *parent)
    : QChartView(chart, parent), isDrawing(false), pathItem(nullptr) {
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
}

void TsnePlot::setScatterSeries(QScatterSeries *series) {
    if (!series)
        return;

    // Clear previous scatter series
    for (QScatterSeries *s : std::as_const(scatterSeriesMap)) {
        if (chart()->series().contains(s)) {
            chart()->removeSeries(s);
            delete s;
        }
    }
    scatterSeriesMap.clear();

    // Map to store label-wise scatter series
    QStringList colorList = {"red", "blue", "green", "purple", "orange", "pink"};
    QMap<QString, QColor> labelColors;
    int colorIndex = 0;

    // Compute min/max values dynamically
    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::min();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::min();

    // Iterate over points and create series per label
    for (int i = 0; i < series->count(); ++i) {
        QPointF point = series->at(i);
        QString label = labels[i];

        // Assign a unique color to each label
        if (!labelColors.contains(label)) {
            labelColors[label] = QColor(colorList[colorIndex % colorList.size()]);
            colorIndex++;
        }

        // Create a new series if label does not exist
        if (!scatterSeriesMap.contains(label)) {
            QScatterSeries *newSeries = new QScatterSeries();
            newSeries->setName(label);
            newSeries->setMarkerSize(8.0);
            newSeries->setColor(labelColors[label]);
            chart()->addSeries(newSeries);
            scatterSeriesMap[label] = newSeries;
        }

        // Add point to respective series
        scatterSeriesMap[label]->append(point);

        // Update min/max values
        minX = std::min(minX, point.x());
        maxX = std::max(maxX, point.x());
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }

    // Create and configure new axes
    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();
    axisX->setRange(minX - 1, maxX + 1);
    axisY->setRange(minY - 1, maxY + 1);

    // Remove existing axes if they exist
    QList<QAbstractAxis *> existingAxes = chart()->axes();
    for (QAbstractAxis *axis : std::as_const(existingAxes)) {
        chart()->removeAxis(axis);
        delete axis;
    }

    chart()->addAxis(axisX, Qt::AlignBottom);
    chart()->addAxis(axisY, Qt::AlignLeft);

    // Attach each series to the new axes
    for (QScatterSeries *labelSeries : std::as_const(scatterSeriesMap)) {
        labelSeries->attachAxis(axisX);
        labelSeries->attachAxis(axisY);
    }

    // Enable legend
    chart()->legend()->setVisible(true);
    chart()->legend()->setAlignment(Qt::AlignRight);
}


void TsnePlot::setLabels(const QStringList &labels) {
    this->labels = labels;
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

    for (auto it = scatterSeriesMap.begin(); it != scatterSeriesMap.end(); ++it) {
        QScatterSeries *series = it.value();
        for (int i = 0; i < series->count(); ++i) {
            QPointF point = series->at(i);
            QPointF screenPos = chart()->mapToPosition(point);

            if (path.contains(screenPos)) {
                qDebug() << "Point inside lasso:" << point;
                series->replace(i, QPointF(point.x(), point.y() + 2));
            }
        }
    }
}
