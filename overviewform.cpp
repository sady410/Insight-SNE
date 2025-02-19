#include "overviewform.h"
#include "ui_overviewform.h"

#include <QVBoxLayout>

OverviewForm::OverviewForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OverviewForm)
{
    ui->setupUi(this);

    QChart *overviewChart = new QChart();
    overviewChart->setTitle("Overview Plot");

    overviewPlot = new OverviewPlot(overviewChart, nullptr);

    // Create vertical layout for plot and buttons
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(overviewPlot);      // Add the plot

    setLayout(layout);
}

OverviewForm::~OverviewForm()
{
    delete ui;
}
