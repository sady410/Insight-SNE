#ifndef TSNEPLOT_H
#define TSNEPLOT_H

#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QGraphicsPathItem>
#include <QMouseEvent>
#include <QPainterPath>
#include <QPen>
#include <QMap>
#include <QList>
#include <QStringList>
#include <QToolTip>
#include <QTimer>

class TsnePlot : public QChartView {
    Q_OBJECT

public:
    explicit TsnePlot(QChart *chart, QWidget *parent = nullptr);
    ~TsnePlot();

    void initPlot(QScatterSeries *series);
    void setLabels(const QStringList &labels);

signals:
    void zoomChanged(qreal minX, qreal maxX, qreal minY, qreal maxY);
    void pointsSelected(const QList<int> &indices);

public slots:
    void resetZoom();
    void decreaseSelectionCount() { selectionCount--; }

    void startHighlightSelection(const QColor &color, const QList<int>& indices);
    void stopHighlightSelection();

private slots:
    void fadeLasso();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    struct QPointFComparator {
        bool operator()(const QPointF &p1, const QPointF &p2) const {
            if (p1.x() != p2.x()) return p1.x() < p2.x();
            return p1.y() < p2.y();
        }
    };

    void setupAxes(qreal minX, qreal maxX, qreal minY, qreal maxY);
    void updateOverviewRectangle();
    void selectPointsInsideLasso();
    void clearLasso();

    QMap<QString, QColor> labelColors;
    QStringList labels;
    QMap<QString, QScatterSeries *> scatterSeriesMap;
    std::map<QPointF, int, QPointFComparator> pointIndexMap;

    QScatterSeries *highlightedSeries;

    bool isDrawing;
    QPainterPath path;
    QGraphicsPathItem *pathItem;
    QPointF firstPoint;
    QList<QPointF> *selectedPoints;
    QList<int> selectedIndices;
    QTimer *lassoFadeTimer;
    qreal lassoOpacity;

    int selectionCount;
    static const int MAX_SELECTIONS = 4;

    qreal initialMinX, initialMaxX, initialMinY, initialMaxY;
};

#endif // TSNEPLOT_H
