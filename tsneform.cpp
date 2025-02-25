#include "tsneform.h"
#include "ui_tsneform.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QChartView>

TsneForm::TsneForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TsneForm)
{
    ui->setupUi(this);

    QChart *tsneChart = new QChart();
    tsneChart->setTitle("Tsne Plot");
    tsnePlot = new TsnePlot(tsneChart, nullptr);

    QPushButton *button1 = new QPushButton("Button 1");
    QPushButton *button2 = new QPushButton("Button 2");
    QPushButton *resetZoomButton = new QPushButton("Reset Zoom");

    button1->setFixedSize(80, 30);
    button2->setFixedSize(80, 30);
    resetZoomButton->setFixedSize(80, 30);

    connect(resetZoomButton, &QPushButton::clicked, tsnePlot, &TsnePlot::resetZoom);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(button1);
    buttonLayout->addWidget(button2);
    buttonLayout->addWidget(resetZoomButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(tsnePlot);

    setLayout(mainLayout);
}

TsneForm::~TsneForm()
{
    delete ui;
}
