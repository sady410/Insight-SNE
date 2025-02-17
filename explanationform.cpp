#include "explanationform.h"
#include "ui_explanationform.h"

#include <QVBoxLayout>
#include <QBarSet>
#include <QBarSeries>
#include <QHorizontalBarSeries>
#include <QChartView>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QRandomGenerator>

ExplanationForm::ExplanationForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ExplanationForm)
{
    ui->setupUi(this);

    QChart *barChart = new QChart();
    QHorizontalBarSeries *barSeries = new QHorizontalBarSeries();

    QBarSet *set = new QBarSet("Values");
    QStringList categories;

    QCategoryAxis *axisY = new QCategoryAxis();

    for (int i = 0; i < 5; ++i) {
        qreal value = QRandomGenerator::global()->bounded(100.0);
        *set << value;
        QString categoryLabel = QString("Category %1").arg(i + 1);
        categories.append(categoryLabel);
        axisY->append(categoryLabel, i + 1);
    }

    barSeries->append(set);
    barChart->addSeries(barSeries);
    barChart->setTitle("Random Horizontal Bar Chart");

    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(0, 100);
    barChart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    barChart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);

    QChartView *chartView = new QChartView(barChart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chartView);

    setLayout(layout);
}

ExplanationForm::~ExplanationForm()
{
    delete ui;
}



