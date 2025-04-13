#ifndef OVERVIEWPLOT_H
#define OVERVIEWPLOT_H

#include "qcustomplot.h"
#include <QVector>

class OverviewPlot : public QCustomPlot {
    Q_OBJECT
public:
    explicit OverviewPlot(QWidget *parent = nullptr);

    // Set scatter plot data
    void setScatterData(const QVector<double> &x, const QVector<double> &y);

public slots:
    // Update the zoom rectangle visualization
    void updateZoomRectangle(double minX, double maxX, double minY, double maxY);

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    // Private helper methods
    void setupPlotAppearance();

    // Zoom rectangle item
    QCPItemRect *m_zoomRect = nullptr;

    // Drag state variables
    bool m_dragging = false;
    QPoint m_dragStart;
};

#endif // OVERVIEWPLOT_H
