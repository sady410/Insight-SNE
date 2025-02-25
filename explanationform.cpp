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

    QChart *explanationChart = new QChart();

    explanationPlot = new ExplanationPlot(explanationChart, nullptr);
    explanationChart->setTitle("Explanation Plot");

    QPushButton *button1 = new QPushButton("Button 1");
    QPushButton *button2 = new QPushButton("Button 2");

    button1->setFixedSize(80, 30);
    button2->setFixedSize(80, 30);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(button1);
    buttonLayout->addWidget(button2);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(explanationPlot);

    setLayout(mainLayout);
}

void ExplanationForm::changeActivePlot() {};

ExplanationForm::~ExplanationForm()
{
    delete ui;
}
