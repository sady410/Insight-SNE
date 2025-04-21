#include "tsneform.h"
#include "ui_tsneform.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

TsneForm::TsneForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TsneForm)
{
    ui->setupUi(this);

    tsnePlot = new TsnePlot(nullptr);

    // Create the reset zoom button
    QPushButton *resetZoomButton = new QPushButton("Reset Zoom", tsnePlot);
    resetZoomButton->setFixedSize(80, 30);
    resetZoomButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 255, 255, 150);"
        "   border: 1px solid black;"
        "}"
        );

    const int topMargin = 5;
    const int leftMargin = 5;
    resetZoomButton->move(leftMargin, topMargin);

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tsnePlot);
    setLayout(mainLayout);

    connect(resetZoomButton, &QPushButton::clicked, tsnePlot, &TsnePlot::resetZoom);
}

TsneForm::~TsneForm()
{
    delete ui;
}
