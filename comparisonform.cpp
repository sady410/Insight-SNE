#include "comparisonform.h"
#include "ui_comparisonform.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QComboBox>

ComparisonForm::ComparisonForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ComparisonForm)
{
    ui->setupUi(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    frame->setLineWidth(2);
    frame->setStyleSheet("background-color: #535353;");

    QVBoxLayout* verticalLayout = new QVBoxLayout(frame);
    verticalLayout->setContentsMargins(8, 8, 8, 8);
    verticalLayout->setSpacing(6);

    // Title and separator with zero bottom margin to make them stick to the top
    QWidget *headerWidget = new QWidget(frame);
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(3);

    QLabel *titleLabel = new QLabel("Selections comparison", headerWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold; color: white; font-size: 16px;");
    headerLayout->addWidget(titleLabel);

    QFrame *separator = new QFrame(headerWidget);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("color: #888;");
    headerLayout->addWidget(separator);

    verticalLayout->addWidget(headerWidget);

    // Add spacing between header and controls
    verticalLayout->addSpacing(15);

    // Add horizontal layout with radio button and two dropdowns separated by minus symbol
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(10);

    QRadioButton *radioButton = new QRadioButton("Toggle", frame);
    radioButton->setStyleSheet("color: white;");
    controlsLayout->addWidget(radioButton);

    QComboBox *dropdown1 = new QComboBox(frame);
    dropdown1->addItem("Option 1");
    dropdown1->addItem("Option 2");
    dropdown1->addItem("Option 3");
    controlsLayout->addWidget(dropdown1);

    QLabel *minusLabel = new QLabel("-", frame);
    minusLabel->setAlignment(Qt::AlignCenter);
    minusLabel->setStyleSheet("color: white; font-weight: bold;");
    controlsLayout->addWidget(minusLabel);

    QComboBox *dropdown2 = new QComboBox(frame);
    dropdown2->addItem("Selection 1");
    dropdown2->addItem("Selection 2");
    dropdown2->addItem("Selection 3");
    controlsLayout->addWidget(dropdown2);

    verticalLayout->addLayout(controlsLayout);

    // Add stretch to push everything to the top
    verticalLayout->addStretch();

    mainLayout->addWidget(frame);
}

ComparisonForm::~ComparisonForm()
{
    delete ui;
}
