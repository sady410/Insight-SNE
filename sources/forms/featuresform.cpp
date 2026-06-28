#include "featuresform.h"
#include "ui_featuresform.h"
#include <QChart>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QFrame>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QButtonGroup>
#include <QHBoxLayout>

FeaturesForm::FeaturesForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FeaturesForm)
{
    ui->setupUi(this);

    // Apply the dark theme styling
    applyStyles();

    // Main layout setup
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Create the main frame
    QFrame *mainFrame = createStyledFrame();
    QVBoxLayout *frameLayout = new QVBoxLayout(mainFrame);
    frameLayout->setContentsMargins(8, 8, 8, 8);
    frameLayout->setSpacing(6);

    // Title and separator at the top
    QLabel *titleLabel = new QLabel("Features", mainFrame);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setStyleSheet("font-weight: bold; color: #4CAF50; font-size: 16px;");
    frameLayout->addWidget(titleLabel);

    QFrame *separator = createSeparator();
    frameLayout->addWidget(separator);

    // Create charts with dark theme styling
    QChart *featuresChart = new QChart();
    featuresChart->setTitle("Mean Features");
    featuresChart->setTitleFont(QFont("Arial", 12, QFont::Bold));
    featuresChart->setTitleBrush(QBrush(QColor("#4CAF50")));
    featuresChart->setBackgroundBrush(QBrush(QColor("#363636")));
    featuresChart->setBackgroundRoundness(10);
    featuresChart->legend()->setLabelColor(QColor("#FFFFFF"));

    QChart *distributionChart = new QChart();
    distributionChart->setTitle("Features Distribution");
    distributionChart->setTitleFont(QFont("Arial", 12, QFont::Bold));
    distributionChart->setTitleBrush(QBrush(QColor("#4CAF50")));
    distributionChart->setBackgroundBrush(QBrush(QColor("#363636")));
    distributionChart->setBackgroundRoundness(10);
    distributionChart->legend()->setLabelColor(QColor("#FFFFFF"));

    // Create plot widgets
    featuresPlot = new BarPlot(featuresChart, nullptr);
    distributionPlot = new DistributionPlot(distributionChart, nullptr);

    // Create dropdowns with consistent styling
    m_selectionsDropdown = new QComboBox();
    m_featuresDropdown = new QComboBox();

    // Apply dropdown styling
    QString dropdownStyle =
        "QComboBox {"
        "   background-color: #424242;"
        "   border: 1px solid #555555;"
        "   border-radius: 5px;"
        "   color: white;"
        "   padding: 5px 10px;"
        "   min-width: 120px;"
        "}"
        "QComboBox:hover {"
        "   border: 1px solid #4CAF50;"
        "}"
        "QComboBox::drop-down {"
        "   subcontrol-origin: padding;"
        "   subcontrol-position: top right;"
        "   width: 20px;"
        "   border-left: 1px solid #555555;"
        "   border-top-right-radius: 5px;"
        "   border-bottom-right-radius: 5px;"
        "}"
        "QComboBox::down-arrow {"
        "   image: url(:/icons/dropdown_arrow.png);"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #363636;"
        "   border: 1px solid #555555;"
        "   border-radius: 5px;"
        "   color: white;"
        "   selection-background-color: #4CAF50;"
        "}";

    m_selectionsDropdown->setStyleSheet(dropdownStyle);
    m_featuresDropdown->setStyleSheet(dropdownStyle);

    // Create buttons with consistent styling
    QPushButton *button1 = new QPushButton("Features");
    QPushButton *button2 = new QPushButton("Distribution");

    // Apply consistent button styling
    QString buttonStyle =
        "QPushButton {"
        "   background-color: #424242;"
        "   border: 1px solid #555555;"
        "   border-radius: 5px;"
        "   color: white;"
        "   font-weight: bold;"
        "   padding: 5px 10px;"
        "   min-width: 100px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4CAF50;"
        "   border: 1px solid #388E3C;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #2E7D32;"
        "}"
        "QPushButton:checked {"
        "   background-color: #4CAF50;"
        "   border: 1px solid #388E3C;"
        "}";

    button1->setStyleSheet(buttonStyle);
    button2->setStyleSheet(buttonStyle);
    button1->setCheckable(true);
    button2->setCheckable(true);
    button1->setChecked(true);

    // Create a button group for toggle behavior
    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(button1);
    buttonGroup->addButton(button2);
    buttonGroup->setExclusive(true);

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
    frameLayout->addLayout(controlsLayout);

    // Prepare the plots container
    QVBoxLayout *plotsLayout = new QVBoxLayout();
    plotsLayout->addWidget(featuresPlot);
    plotsLayout->addWidget(distributionPlot);
    frameLayout->addLayout(plotsLayout);

    // Add main frame to layout
    mainLayout->addWidget(mainFrame);

    // Initialize visibility
    featuresPlot->setVisible(true);
    distributionPlot->setVisible(false);
    m_selectionsDropdown->setVisible(false);
    m_featuresDropdown->setVisible(false);

    // Connect buttons
    connect(button1, &QPushButton::clicked, this, &FeaturesForm::showFeaturesPlot);
    connect(button2, &QPushButton::clicked, this, &FeaturesForm::showDistributionPlot);

    // Connect dropdown signals
    connect(m_selectionsDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FeaturesForm::onSelectionChanged);
    connect(m_featuresDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FeaturesForm::onFeatureChanged);

    // Connect our internal signals
    connect(this, &FeaturesForm::selectionChanged,
            this, &FeaturesForm::handleSelectionChanged);
    connect(this, &FeaturesForm::featureChanged,
            this, &FeaturesForm::handleFeatureChanged);
}

void FeaturesForm::applyStyles()
{
    // Apply drop shadow to the widget
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 120));
    shadowEffect->setOffset(0, 2);
    this->setGraphicsEffect(shadowEffect);

    // Set the widget style sheet
    this->setStyleSheet(
        "FeaturesForm {"
        "   background-color: #1d1d1d;"
        "   border-radius: 8px;"
        "   color: #e6e6e6;"
        "}"
        "QLabel#titleLabel {"
        "   color: #4CAF50;"
        "   font-size: 15px;"
        "   font-weight: bold;"
        "   padding: 8px;"
        "   background-color: #333333;"
        "   border-radius: 4px;"
        "   margin-bottom: 3px;"
        "}"
        "QChartView {"
        "   background-color: #363636;"
        "   border: 1px solid #555555;"
        "   border-radius: 5px;"
        "}"
        "QScrollBar:vertical {"
        "   border: none;"
        "   background-color: #2d2d2d;"
        "   width: 12px;"
        "   margin: 0px;"
        "   border-radius: 6px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background-color: #4CAF50;"
        "   min-height: 35px;"
        "   border-radius: 6px;"
        "   margin: 2px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background-color: #66BB6A;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "   background-color: #2d2d2d;"
        "   border-radius: 6px;"
        "}"
        );
}

void FeaturesForm::setDataManager(DataManagerPtr dataManager)
{
    m_dataManager = dataManager;
    if (m_dataManager && m_dataManager->isDataLoaded()) {
        setFeatureNames(m_dataManager->featureNames());
    }
}

QFrame* FeaturesForm::createStyledFrame()
{
    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    frame->setLineWidth(2);
    frame->setStyleSheet(
        "QFrame {"
        "   background-color: #2d2d2d;"
        "   border-radius: 8px;"
        "   border: 1px solid #444444;"
        "}"
        );

    // Add drop shadow to frame
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect;
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setOffset(0, 2);
    frame->setGraphicsEffect(shadowEffect);

    return frame;
}

QFrame* FeaturesForm::createSeparator()
{
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet(
        "color: #555555; "
        "background-color: #555555; "
        "margin: 5px 0px;"
        );
    return separator;
}

FeaturesForm::~FeaturesForm()
{
    delete ui;
}

void FeaturesForm::setFeatureNames(QStringList featureNames) {
    m_featuresDropdown->clear();
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
