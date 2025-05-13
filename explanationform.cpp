#include "explanationform.h"
#include "ui_explanationform.h"
#include <QVBoxLayout>
#include <QBarSet>
#include <QBarSeries>
#include <QChartView>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QRandomGenerator>
#include <QPushButton>
#include <QFrame>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QButtonGroup>

ExplanationForm::ExplanationForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ExplanationForm)
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
    QLabel *titleLabel = new QLabel("Explanation", mainFrame);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setStyleSheet("font-weight: bold; color: #4CAF50; font-size: 16px;");
    frameLayout->addWidget(titleLabel);

    QFrame *separator = createSeparator();
    frameLayout->addWidget(separator);

    // Button layout with consistent styling
    QPushButton *button1 = new QPushButton("Magnitudes");
    QPushButton *button2 = new QPushButton("Gradients");

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

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(button1);
    buttonGroup->addButton(button2);
    buttonGroup->setExclusive(true);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(button1);
    buttonLayout->addWidget(button2);
    buttonLayout->addStretch();
    frameLayout->addLayout(buttonLayout);

    // Create a stacked widget to hold both plot containers
    QStackedWidget *stackedWidget = new QStackedWidget(mainFrame);

    // Create charts with dark theme styling
    QChart *magnitudesChart = new QChart();
    magnitudesChart->setTitle("Magnitudes");
    magnitudesChart->setTitleFont(QFont("Arial", 12, QFont::Bold));
    magnitudesChart->setTitleBrush(QBrush(QColor("#4CAF50")));
    magnitudesChart->setBackgroundBrush(QBrush(QColor("#363636")));
    magnitudesChart->setBackgroundRoundness(10);
    magnitudesChart->legend()->setLabelColor(QColor("#FFFFFF"));

    QChart *gradientsChartTop = new QChart();
    gradientsChartTop->setTitle("X-axis");
    gradientsChartTop->setTitleFont(QFont("Arial", 12, QFont::Bold));
    gradientsChartTop->setTitleBrush(QBrush(QColor("#4CAF50")));
    gradientsChartTop->setBackgroundBrush(QBrush(QColor("#363636")));
    gradientsChartTop->setBackgroundRoundness(10);
    gradientsChartTop->legend()->setLabelColor(QColor("#FFFFFF"));

    QChart *gradientsChartBottom = new QChart();
    gradientsChartBottom->setTitle("Y-axis");
    gradientsChartBottom->setTitleFont(QFont("Arial", 12, QFont::Bold));
    gradientsChartBottom->setTitleBrush(QBrush(QColor("#4CAF50")));
    gradientsChartBottom->setBackgroundBrush(QBrush(QColor("#363636")));
    gradientsChartBottom->setBackgroundRoundness(10);
    gradientsChartBottom->legend()->setLabelColor(QColor("#FFFFFF"));

    // Create plot widgets
    magnitudesPlot = new BarPlot(magnitudesChart, nullptr);
    gradientsPlotTop = new BarPlot(gradientsChartTop, nullptr);
    gradientsPlotBottom = new BarPlot(gradientsChartBottom, nullptr);

    // Create containers for each plot type
    QWidget *magnitudesContainer = new QWidget();
    QVBoxLayout *magnitudesLayout = new QVBoxLayout(magnitudesContainer);
    magnitudesLayout->setContentsMargins(0, 0, 0, 0);
    magnitudesLayout->addWidget(magnitudesPlot);

    QWidget *gradientsContainer = new QWidget();
    QVBoxLayout *gradientsLayout = new QVBoxLayout(gradientsContainer);
    gradientsLayout->setContentsMargins(0, 0, 0, 0);
    gradientsLayout->addWidget(gradientsPlotTop, 1);
    gradientsLayout->addWidget(gradientsPlotBottom, 1);

    // Add containers to stacked widget
    stackedWidget->addWidget(magnitudesContainer);
    stackedWidget->addWidget(gradientsContainer);

    // Add stacked widget to main layout
    frameLayout->addWidget(stackedWidget);

    // Connect buttons to switch between plots
    connect(button1, &QPushButton::clicked, this, [this, stackedWidget]() {
        stackedWidget->setCurrentIndex(0);
    });

    connect(button2, &QPushButton::clicked, this, [this, stackedWidget]() {
        stackedWidget->setCurrentIndex(1);
    });

    // Add main frame to layout
    mainLayout->addWidget(mainFrame);
}

void ExplanationForm::applyStyles()
{
    // Apply drop shadow to the widget
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 120));
    shadowEffect->setOffset(0, 2);
    this->setGraphicsEffect(shadowEffect);

    // Set the widget style sheet
    this->setStyleSheet(
        "ExplanationForm {"
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

QFrame* ExplanationForm::createStyledFrame()
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

QFrame* ExplanationForm::createSeparator()
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

ExplanationForm::~ExplanationForm() {
    delete ui;
}
