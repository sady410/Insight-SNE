#include "overviewform.h"
#include "ui_overviewform.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QGraphicsDropShadowEffect>

OverviewForm::OverviewForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OverviewForm)
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
    QLabel *titleLabel = new QLabel("Overview", mainFrame);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setStyleSheet("font-weight: bold; color: #4CAF50; font-size: 16px;");
    frameLayout->addWidget(titleLabel);

    QFrame *separator = createSeparator();
    frameLayout->addWidget(separator);

    // Create info label (initially empty)
    m_infoLabel = new QLabel(mainFrame);
    m_infoLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_infoLabel->setObjectName("infoLabel");
    m_infoLabel->setStyleSheet("font-size: 12px; padding: 8px; color: #e6e6e6; background-color: #363636; border-radius: 4px; border: 1px solid #555555;");
    m_infoLabel->setWordWrap(true);
    m_infoLabel->setMaximumHeight(80); // Limit height to avoid taking too much space
    m_infoLabel->setVisible(false); // Initially hidden until there's content
    frameLayout->addWidget(m_infoLabel);

    // Create the plot with styled background
    overviewPlot = new OverviewPlot(nullptr);
    frameLayout->addWidget(overviewPlot, 1);

    // Add main frame to layout
    mainLayout->addWidget(mainFrame);
}

void OverviewForm::applyStyles()
{
    // Apply drop shadow to the widget
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 120));
    shadowEffect->setOffset(0, 2);
    this->setGraphicsEffect(shadowEffect);

    // Set the widget style sheet
    this->setStyleSheet(
        "OverviewForm {"
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

QFrame* OverviewForm::createStyledFrame()
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

QFrame* OverviewForm::createSeparator()
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

void OverviewForm::setDatasetInfo(const QString &info)
{
    // Set the text for the info label
    m_infoLabel->setText(info);
    m_infoLabel->setVisible(!info.isEmpty()); // Only show if there's text
}

OverviewForm::~OverviewForm()
{
    delete ui;
}
