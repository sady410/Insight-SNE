#include "explanationform.h"
#include "ui_explanationform.h"

#include <QVBoxLayout>
#include <QBarSet>
#include <QBarSeries>
#include <QChartView>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QRandomGenerator>

ExplanationForm::ExplanationForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ExplanationForm)
{
    ui->setupUi(this);

    QChart *explanationChart = new QChart();

    explanationPlot = new ExplanationPlot(explanationChart, nullptr);
    explanationChart->setTitle("Explanation Plot");

    // QBarSet *set = new QBarSet("Features");

    // TODO missing code

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(explanationPlot);
    setLayout(layout);
}

void ExplanationForm::setGradients(QList<QList<qreal>> *gradients) {
    this->gradients = gradients;
}

// void ExplanationForm::setHorizontalBarSeries(QHorizontalBarSeries *horizontalBarSeries) {

// }

ExplanationForm::~ExplanationForm()
{
    delete ui;
}


// QCategoryAxis *axisY = new QCategoryAxis();

// for (int i = 0; i < 5; ++i) {
//     qreal value = QRandomGenerator::global()->bounded(100.0);
//     *set << value;
//     QString categoryLabel = QString("Category %1").arg(i + 1);
//     categories.append(categoryLabel);
//     axisY->append(categoryLabel, i + 1);
// }

// barSeries->append(set);
// barChart->addSeries(barSeries);

// QValueAxis *axisX = new QValueAxis();
// axisX->setRange(0, 100);
// barChart->addAxis(axisX, Qt::AlignBottom);
// barSeries->attachAxis(axisX);

// barChart->addAxis(axisY, Qt::AlignLeft);
// barSeries->attachAxis(axisY);

