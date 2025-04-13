#include "overviewform.h"
#include "ui_overviewform.h"

#include <QVBoxLayout>

OverviewForm::OverviewForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OverviewForm)
{
    ui->setupUi(this);

    overviewPlot = new OverviewPlot(nullptr);

    // Create vertical layout for plot and buttons
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(overviewPlot);      // Add the plot

    setLayout(layout);
}

OverviewForm::~OverviewForm()
{
    delete ui;
}
