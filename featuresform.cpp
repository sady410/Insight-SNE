#include "featuresform.h"
#include "ui_featuresform.h"
#include <QChart>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>  // Added for dropdowns

FeaturesForm::FeaturesForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FeaturesForm)
{
    ui->setupUi(this);
    QChart *featuresChart = new QChart();
    featuresChart->setTitle("Features Plot");
    featuresPlot = new BarPlot(featuresChart, nullptr);
    QChart *distributionChart = new QChart();
    distributionChart->setTitle("Distribution Plot");
    distributionPlot = new DistributionPlot(distributionChart, nullptr);

    // Create buttons
    QPushButton *button1 = new QPushButton("Features");
    QPushButton *button2 = new QPushButton("Distribution");
    button1->setFixedSize(80, 30);
    button2->setFixedSize(80, 30);
    connect(button1, &QPushButton::clicked, this, &FeaturesForm::showFeaturesPlot);
    connect(button2, &QPushButton::clicked, this, &FeaturesForm::showDistributionPlot);

    // Create dropdowns
    m_selectionsDropdown = new QComboBox();
    m_featuresDropdown = new QComboBox();
    m_selectionsDropdown->addItem("Dropdown 1 Option 1");
    m_selectionsDropdown->addItem("Dropdown 1 Option 2");
    m_featuresDropdown->addItem("Dropdown 2 Option 1");
    m_featuresDropdown->addItem("Dropdown 2 Option 2");

    // Initially hide dropdowns since we start with features plot
    m_selectionsDropdown->setVisible(false);
    m_featuresDropdown->setVisible(false);

    // Create layout for dropdowns (left side)
    QHBoxLayout *dropdownLayout = new QHBoxLayout();
    dropdownLayout->addWidget(m_selectionsDropdown);
    dropdownLayout->addWidget(m_featuresDropdown);
    dropdownLayout->addStretch(); // This pushes dropdowns to the left

    // Create layout for buttons (right side)
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(); // This pushes buttons to the right
    buttonLayout->addWidget(button1);
    buttonLayout->addWidget(button2);

    // Combine dropdowns and buttons in one row
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->addLayout(dropdownLayout);
    controlsLayout->addLayout(buttonLayout);

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(controlsLayout);
    mainLayout->addWidget(featuresPlot);
    mainLayout->addWidget(distributionPlot);

    featuresPlot->setVisible(true);
    distributionPlot->setVisible(false);
    setLayout(mainLayout);
}

FeaturesForm::~FeaturesForm()
{
    delete ui;
}

void FeaturesForm::showFeaturesPlot() {
    featuresPlot->setVisible(true);
    distributionPlot->setVisible(false);

    // Hide dropdowns for features plot
    m_selectionsDropdown->setVisible(false);
    m_featuresDropdown->setVisible(false);
}

void FeaturesForm::showDistributionPlot() {
    featuresPlot->setVisible(false);
    distributionPlot->setVisible(true);

    // Show dropdowns for distribution plot
    m_selectionsDropdown->setVisible(true);
    m_featuresDropdown->setVisible(true);
}
