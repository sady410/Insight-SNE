#include "tsneform.h"
#include "ui_tsneform.h"

#include <QVBoxLayout>
#include <QChartView>
#include <QValueAxis>
#include <QScatterSeries>

TsneForm::TsneForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TsneForm)
    , data(nullptr)
{
    ui->setupUi(this);

    QChart *tsneChart = new QChart();
    tsneChart->setTitle("Tsne Plot");

    tsnePlot = new TsnePlot(tsneChart, nullptr);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tsnePlot);
    setLayout(layout);
}

void TsneForm::setScatterSeries(QScatterSeries *scatterSeries)
{
    if (!scatterSeries)
        return;

    data = scatterSeries;
    QChart *tsneChart = tsnePlot->chart();

    tsneChart->addSeries(data);
    data->setMarkerSize(8.0);

    // Dynamically calculate min/max for the axes
    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::min();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::min();

    for (const QPointF &point : scatterSeries->points()) {
        minX = std::min(minX, point.x());
        maxX = std::max(maxX, point.x());
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }

    // Set axis ranges with some margin
    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();
    axisX->setRange(minX - 1, maxX + 1);  // Adding some margin around the data
    axisY->setRange(minY - 1, maxY + 1);

    tsneChart->addAxis(axisX, Qt::AlignBottom);
    tsneChart->addAxis(axisY, Qt::AlignLeft);
    data->attachAxis(axisX);
    data->attachAxis(axisY);
}

TsneForm::~TsneForm()
{
    delete ui;
}
