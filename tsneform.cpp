#include "tsneform.h"
#include "ui_tsneform.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
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

    QPushButton *resetZoomButton = new QPushButton("Reset Zoom");
    resetZoomButton->setFixedSize(80, 30); // Fixed size for Reset Zoom button

    // Create a horizontal layout for the top bar (reset zoom)
    QHBoxLayout *topBarLayout = new QHBoxLayout();

    // Add a stretchable spacer to push the Reset Zoom button to the right
    topBarLayout->addStretch();
    // Add the Reset Zoom button to the right
    topBarLayout->addWidget(resetZoomButton);

    // Main layout for the form
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topBarLayout); // Add the top bar layout
    mainLayout->addWidget(tsnePlot);    // Add the plot below the top bar

    setLayout(mainLayout);

    // Connect signals and slots
    connect(resetZoomButton, &QPushButton::clicked, tsnePlot, &TsnePlot::resetZoom);
}

TsneForm::~TsneForm()
{
    delete ui;
}
