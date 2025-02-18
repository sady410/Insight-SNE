#include "tsneform.h"
#include "ui_tsneform.h"

#include <QVBoxLayout>
#include <QChartView>

TsneForm::TsneForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TsneForm)
{
    ui->setupUi(this);

    QChart *tsneChart = new QChart();
    tsneChart->setTitle("Tsne Plot");

    tsnePlot = new TsnePlot(tsneChart, nullptr);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tsnePlot);
    setLayout(layout);
}

TsneForm::~TsneForm()
{
    delete ui;
}
