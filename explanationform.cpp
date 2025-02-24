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
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(explanationPlot);
}

void ExplanationForm::changeActivePlot() {};

ExplanationForm::~ExplanationForm()
{
    delete ui;
}
