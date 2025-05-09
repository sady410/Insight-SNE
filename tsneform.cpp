#include "tsneform.h"
#include "ui_tsneform.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QGraphicsDropShadowEffect>

TsneForm::TsneForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TsneForm)
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

    // Title and separator - these stay at the top
    QLabel *titleLabel = new QLabel("T-SNE Plot", mainFrame);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setStyleSheet("font-weight: bold; color: #4CAF50; font-size: 16px;");
    frameLayout->addWidget(titleLabel);

    QFrame *separator = createSeparator();
    frameLayout->addWidget(separator);

    // Create the plot container frame
    QFrame *plotFrame = new QFrame(mainFrame);
    plotFrame->setFrameStyle(QFrame::NoFrame);
    plotFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *plotLayout = new QVBoxLayout(plotFrame);
    plotLayout->setContentsMargins(0, 0, 0, 0);

    // Create the tSNE plot
    tsnePlot = new TsnePlot(plotFrame);
    plotLayout->addWidget(tsnePlot);

    // Add the plot container to the main layout
    frameLayout->addWidget(plotFrame);

    // Create the reset zoom button with matching style
    QPushButton *resetZoomButton = new QPushButton("Reset Zoom", mainFrame);
    resetZoomButton->setFixedSize(100, 30);
    resetZoomButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #424242;"
        "   border: 1px solid #555555;"
        "   border-radius: 5px;"
        "   color: white;"
        "   font-weight: bold;"
        "   padding: 5px 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4CAF50;"
        "   border: 1px solid #388E3C;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #2E7D32;"
        "}"
        );

    // Position the reset button at the top-left corner of the plot area
    resetZoomButton->setParent(tsnePlot);
    resetZoomButton->move(10, 10);
    resetZoomButton->raise();

    // Add the main frame to the layout
    mainLayout->addWidget(mainFrame);

    // Connect signals
    connect(resetZoomButton, &QPushButton::clicked, tsnePlot, &TsnePlot::resetZoom);
}

void TsneForm::applyStyles()
{
    // Apply drop shadow to the widget
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 120));
    shadowEffect->setOffset(0, 2);
    this->setGraphicsEffect(shadowEffect);

    // Set the widget style sheet
    this->setStyleSheet(
        "TsneForm {"
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

QFrame* TsneForm::createStyledFrame()
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

QFrame* TsneForm::createSeparator()
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

TsneForm::~TsneForm()
{
    delete ui;
}
