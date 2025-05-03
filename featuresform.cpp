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

    // Connect dropdown signals
    connect(m_selectionsDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FeaturesForm::onSelectionChanged);
    connect(m_featuresDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FeaturesForm::onFeatureChanged);

    // Connect our internal signals to the distribution plot
    connect(this, &FeaturesForm::selectionChanged,
            this, &FeaturesForm::handleSelectionChanged);
    connect(this, &FeaturesForm::featureChanged,
            this, &FeaturesForm::handleFeatureChanged);

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

void FeaturesForm::setFeatureNames(QStringList featureNames) {
    m_featuresDropdown->addItems(featureNames);
}

void FeaturesForm::addSelection(int id, const QColor &color, const QList<int>& indices, const bool enableDelete, const bool state, const QString name) {
    // Mark unused parameters
    Q_UNUSED(color);
    Q_UNUSED(indices);
    Q_UNUSED(enableDelete);
    Q_UNUSED(state);

    m_selectionsMap[id] = name;
    m_selectionsDropdown->addItem(name);
}

void FeaturesForm::removeSelection(int id) {
    // Remove the selection from dropdown and map
    if (m_selectionsMap.contains(id)) {
        int index = m_selectionsDropdown->findText(m_selectionsMap[id]);
        if (index >= 0) {
            m_selectionsDropdown->removeItem(index);
        }
        m_selectionsMap.remove(id);
    }
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

void FeaturesForm::onSelectionChanged(int index) {
    if (index >= 0) {
        QString selectionName = m_selectionsDropdown->itemText(index);
        emit selectionChanged(selectionName);
    }
}

void FeaturesForm::onFeatureChanged(int index) {
    if (index >= 0) {
        QString featureName = m_featuresDropdown->itemText(index);
        emit featureChanged(featureName);
    }
}

void FeaturesForm::handleSelectionChanged(const QString& selectionName) {
    // Find the selection ID from the name
    int selectionId = -1;
    for (auto it = m_selectionsMap.begin(); it != m_selectionsMap.end(); ++it) {
        if (it.value() == selectionName) {
            selectionId = it.key();
            break;
        }
    }

    // Update the distribution plot with the selected ID
    distributionPlot->setCurrentSelection(selectionId);
}

void FeaturesForm::handleFeatureChanged(const QString& featureName) {
    // Update the distribution plot with the selected feature
    distributionPlot->setCurrentFeature(featureName);
}
