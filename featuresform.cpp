#include "featuresform.h"
#include "ui_featuresform.h"

#include <QChart>
#include <QVBoxLayout>

FeaturesForm::FeaturesForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FeaturesForm)
{
    ui->setupUi(this);

    QChart *featuresChart = new QChart();
    featuresChart->setTitle("Features Plot");
    featuresPlot = new BarPlot(featuresChart, nullptr);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(featuresPlot);

    setLayout(layout);
}

FeaturesForm::~FeaturesForm()
{
    delete ui;
}
