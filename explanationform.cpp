#include "explanationform.h"
#include "ui_explanationform.h"

#include <QVBoxLayout>
#include <QBarSet>
#include <QBarSeries>
#include <QChartView>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QRandomGenerator>
#include <QPushButton>

ExplanationForm::ExplanationForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ExplanationForm)
{
    ui->setupUi(this);

    QChart *magnitudesChart = new QChart();

    magnitudesPlot = new MagnitudesPlot(magnitudesChart, nullptr);
    magnitudesChart->setTitle("Magnitudes Plot");

    QChart *gradientsChartTop = new QChart();

    gradientsPlotTop = new GradientsPlot(gradientsChartTop, nullptr);
    gradientsChartTop->setTitle("Gradients Plot X");

    QChart *gradientsChartBottom = new QChart();

    gradientsPlotBottom = new GradientsPlot(gradientsChartBottom, nullptr);
    gradientsChartBottom->setTitle("Gradients Plot Y");

    gradientsPlotTop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gradientsPlotBottom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QPushButton *button1 = new QPushButton("Magnitudes");
    QPushButton *button2 = new QPushButton("Gradients");

    button1->setFixedSize(80, 30);
    button2->setFixedSize(80, 30);

    connect(button1, &QPushButton::clicked, this, &ExplanationForm::showMagnitudesPlot);
    connect(button2, &QPushButton::clicked, this, &ExplanationForm::showGradientsPlot);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(button1);
    buttonLayout->addWidget(button2);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(magnitudesPlot);
    mainLayout->addWidget(gradientsPlotTop, 1);
    mainLayout->addWidget(gradientsPlotBottom, 1);

    magnitudesPlot->setVisible(true);
    gradientsPlotTop->setVisible(false);
    gradientsPlotBottom->setVisible(false);

    setLayout(mainLayout);
}

void ExplanationForm::showMagnitudesPlot() {
    magnitudesPlot->setVisible(true);
    gradientsPlotTop->setVisible(false);
    gradientsPlotBottom->setVisible(false);
}

void ExplanationForm::showGradientsPlot() {
    magnitudesPlot->setVisible(false);
    gradientsPlotTop->setVisible(true);
    gradientsPlotBottom->setVisible(true);
}

ExplanationForm::~ExplanationForm() {
    delete ui;
}
