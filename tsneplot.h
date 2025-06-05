#ifndef TSNEPLOT_H
#define TSNEPLOT_H

#include "qcustomplot.h"
#include "datamanager.h"
#include <QMouseEvent>
#include <QPainterPath>
#include <QPen>
#include <QMap>
#include <QList>
#include <QStringList>
#include <QToolTip>
#include <QTimer>
#include <QVector>

class TsnePlot : public QCustomPlot {
    Q_OBJECT

public:
    explicit TsnePlot(QWidget *parent = nullptr);
    ~TsnePlot();

    enum SelectionMode {
        SinglePointMode,
        LassoMode
    };
    Q_ENUM(SelectionMode)
    void setSelectionMode(SelectionMode mode);
    SelectionMode selectionMode() const { return m_selectionMode; }

    void initPlot(bool hasClassLabels, bool hasUniqueLabels);

    // Set the data manager
    void setDataManager(DataManagerPtr dataManager);

signals:
    void zoomChanged(qreal minX, qreal maxX, qreal minY, qreal maxY);
    void pointsSelected(const QList<int> &indices);
    void pointClicked(int id);

public slots:
    void resetZoom();
    void decreaseSelectionCount() { m_selectionCount--; }

    // Highlighting
    void startHighlightSelection(const QColor &color, const QList<int>& indices);
    void stopHighlightSelection();

    // Arrows
    void addArrows(int id, const QList<int>& indices, const QString &featureName, const QColor &color);
    void removeArrows(int id);
    void changeArrowsColor(int id, const QColor &newColor);

    // Contour
    void drawContourPlot(const QString &featureName, const QString &dataSource);
    void clearContourPlot();
    // Scope
    void drawScopeContourPlot(const QString &selectedPointId);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void fadeLasso();
    void handleZoomChanged(const QCPRange &newRange);

private:
    struct QPointFComparator {
        bool operator()(const QPointF &p1, const QPointF &p2) const {
            if (p1.x() != p2.x()) return p1.x() < p2.x();
            return p1.y() < p2.y();
        }
    };

    // For selection mode
    SelectionMode m_selectionMode;
    int m_pointHoveredId;

    // For scope
    int m_selectedInstanceForScope = 0;

    void updateOverviewRectangle();
    void selectPointsInsideLasso();
    void clearLasso();
    QPointF coordsToPixels(double x, double y) const;

    QMap<QString, QColor> m_labelColors;
    QMap<QString, QCPGraph*> m_scatterGraphsMap;
    std::map<QPointF, int, QPointFComparator> m_pointIndexMap;

    // For Panning
    bool m_isPanning;
    QPoint m_panStartPos;

    // For contour plotting
    QCPColorMap *m_colorMap;
    QCPColorScale *m_colorScale;
    QCPColorMapData* createContourData(const QString &featureName, const QString &dataSource);

    QString m_currentContourFeature;
    QString m_currentDataSource;
    QList<QList<qreal>> m_zValues;

    // Data manager
    DataManagerPtr m_dataManager;

    // For arrows
    struct ArrowData {
        int id;
        QPointF startPoint;
        QPointF vector;
        QString featureName;
        QColor color;
    };
    QList<ArrowData> m_arrowData;
    QList<QCPItemLine*> m_arrowItems;

    // For highlighting
    QCPGraph *m_highlightedGraph;
    QList<QCPGraph*> m_highlightGraphs; // Graphs for highlighted points
    QMap<QString, QCPGraph*> m_shadowGraphs; // Graphs for shadowed points
    QList<int> m_highlightedIndices; // Currently highlighted point indices
    QColor m_highlightColor; // Color used for highlighting

    // For lasso selection
    bool m_isDrawing;
    QVector<QPoint> m_lassoPoints;
    QCPCurve* m_lassoCurve;
    QList<QPointF> *m_selectedPoints;
    QList<int> m_selectedIndices;
    QTimer *m_lassoFadeTimer;
    qreal m_lassoOpacity;
    int m_selectionCount;
    static const int MAX_SELECTIONS = 4;

    qreal m_initialMinX, m_initialMaxX, m_initialMinY, m_initialMaxY;
};

#endif // TSNEPLOT_H
