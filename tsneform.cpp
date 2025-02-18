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

    // Create the t-SNE chart
    QChart *tsneChart = new QChart();
    tsneChart->setTitle("Tsne Plot");

    tsnePlot = new TsnePlot(tsneChart, nullptr);

    // Create buttons
    QPushButton *button1 = new QPushButton("Button 1");
    QPushButton *button2 = new QPushButton("Button 2");
    QPushButton *button3 = new QPushButton("Button 3");

    // Set smaller size for buttons
    button1->setFixedSize(80, 30);
    button2->setFixedSize(80, 30);
    button3->setFixedSize(80, 30);

    // Create horizontal layout for buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();  // Push buttons to the right
    buttonLayout->addWidget(button1);
    buttonLayout->addWidget(button2);
    buttonLayout->addWidget(button3);

    // Create vertical layout for plot and buttons
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(buttonLayout);  // Add buttons layout
    mainLayout->addWidget(tsnePlot);      // Add the plot

    setLayout(mainLayout);
}

TsneForm::~TsneForm()
{
    delete ui;
}
