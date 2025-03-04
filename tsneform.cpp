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

    // Create a horizontal layout for the top bar (selection buttons + reset zoom)
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
    connect(tsnePlot, &TsnePlot::pointsSelected, this, &TsneForm::handleSelection);
}

TsneForm::~TsneForm()
{
    delete ui;
}


void TsneForm::handleSelection(const QList<int>& selectedIndices, const QColor& color)
{
    // Create a small button (like an icon) for the selection
    QPushButton *selectionButton = new QPushButton();
    selectionButton->setFixedSize(30, 30); // Small fixed size (like an icon)
    selectionButton->setStyleSheet(QString("background-color: %1; border: none;").arg(color.name()));
    selectionButton->setToolTip(QString("Selection %1").arg(selectedIndices.size()));

    // Store the selected indices in the button's property
    selectionButton->setProperty("selectedIndices", QVariant::fromValue(selectedIndices));

    // Add the button to the top bar layout (left side)
    QHBoxLayout *topBarLayout = qobject_cast<QHBoxLayout *>(layout()->itemAt(0)->layout());
    if (topBarLayout) {
        topBarLayout->insertWidget(topBarLayout->count() - 1, selectionButton); // Insert before the spacer
    }
}
