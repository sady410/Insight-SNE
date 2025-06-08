#include "comparisonform.h"
#include "ui_comparisonform.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QGraphicsDropShadowEffect>
#include <QFrame>

ComparisonForm::ComparisonForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ComparisonForm)
{
    ui->setupUi(this);

    // Apply the dark theme styling
    applyStyles();

    // Set up main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    this->setLayout(mainLayout);

    // Create main frame
    QFrame *frame = createStyledFrame();
    QVBoxLayout* verticalLayout = new QVBoxLayout(frame);
    verticalLayout->setContentsMargins(8, 8, 8, 8);
    verticalLayout->setSpacing(6);

    // Title and separator
    QLabel *titleLabel = new QLabel("Selections Comparison", frame);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setStyleSheet("font-weight: bold; color: #4CAF50; font-size: 16px;");
    verticalLayout->addWidget(titleLabel);

    QFrame *separator = createSeparator();
    verticalLayout->addWidget(separator);

    // Add spacing between header and controls
    verticalLayout->addSpacing(8);

    // Add horizontal layout with checkbox and two dropdowns separated by label
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(10);

    // Create styled checkbox
    QCheckBox *compareCheckBox = new QCheckBox("Compare", frame);
    compareCheckBox->setChecked(false);
    controlsLayout->addWidget(compareCheckBox);

    // Create first dropdown with styling
    m_firstSelectionDropdown = new QComboBox(frame);
    controlsLayout->addWidget(m_firstSelectionDropdown);

    // Create styled divider label
    QLabel *diffLabel = new QLabel("vs", frame);
    diffLabel->setAlignment(Qt::AlignCenter);
    diffLabel->setStyleSheet("color: #4CAF50; font-weight: bold; font-size: 14px;");
    controlsLayout->addWidget(diffLabel);

    // Create second dropdown with styling
    m_secondSelectionDropdown = new QComboBox(frame);
    controlsLayout->addWidget(m_secondSelectionDropdown);

    verticalLayout->addLayout(controlsLayout);

    // Add stretch to push everything to the top
    verticalLayout->addStretch();

    mainLayout->addWidget(frame);

    // Connect signals
    connect(m_firstSelectionDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ComparisonForm::onSelectionChanged);
    connect(m_secondSelectionDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ComparisonForm::onSelectionChanged);
    connect(compareCheckBox, &QCheckBox::toggled, this, &ComparisonForm::onCompareToggled);
}

void ComparisonForm::applyStyles()
{
    // Apply drop shadow to the widget
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 120));
    shadowEffect->setOffset(0, 2);
    this->setGraphicsEffect(shadowEffect);

    // Set the widget style sheet
    this->setStyleSheet(
        "ComparisonForm {"
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
        "QCheckBox {"
        "   spacing: 8px;"
        "   color: white;"
        "   font-size: 13px;"
        "}"
        "QCheckBox::indicator {"
        "   width: 18px;"
        "   height: 18px;"
        "   border-radius: 3px;"
        "}"
        "QCheckBox::indicator:unchecked {"
        "   border: 2px solid #555555;"
        "   background-color: #424242;"
        "}"
        "QCheckBox::indicator:unchecked:hover {"
        "   border: 2px solid #4CAF50;"
        "}"
        "QCheckBox::indicator:checked {"
        "   border: 2px solid #4CAF50;"
        "   background-color: #4CAF50;"
        "   image: url('data:image/svg+xml;utf8,<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"12\" height=\"12\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"white\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><polyline points=\"20 6 9 17 4 12\"></polyline></svg>');"
        "   background-position: center;"
        "   background-repeat: no-repeat;"
        "}"
        "QComboBox {"
        "   border: 1px solid #555555;"
        "   border-radius: 5px;"
        "   padding: 6px 20px 6px 12px;"
        "   min-width: 8em;"
        "   background-color: #424242;"
        "   min-height: 28px;"
        "   color: white;"
        "   font-size: 13px;"
        "}"
        "QComboBox:hover {"
        "   border: 1px solid #4CAF50;"
        "   background-color: #484848;"
        "}"
        "QComboBox:disabled {"
        "   background-color: #323232;"
        "   color: #9E9E9E;"
        "}"
        "QComboBox::drop-down {"
        "   subcontrol-origin: padding;"
        "   subcontrol-position: top right;"
        "   width: 22px;"
        "   border-left-width: 1px;"
        "   border-left-color: #555555;"
        "   border-left-style: solid;"
        "   border-top-right-radius: 5px;"
        "   border-bottom-right-radius: 5px;"
        "}"
        "QComboBox::down-arrow {"
        "   image: url('data:image/svg+xml;utf8,<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"12\" height=\"12\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"white\" stroke-width=\"2\"><path d=\"M6 9l6 6 6-6\"/></svg>');"
        "   width: 12px;"
        "   height: 12px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #424242;"
        "   border: 1px solid #555555;"
        "   border-radius: 4px;"
        "   selection-background-color: #4CAF50;"
        "   selection-color: white;"
        "   padding: 2px;"
        "}"
        );
}

QFrame* ComparisonForm::createStyledFrame()
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

QFrame* ComparisonForm::createSeparator()
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

void ComparisonForm::addSelection(int id, const QColor &color, const QList<int>& indices, const bool enableDelete, const bool state, const QString name) {
    // Mark unused parameters
    Q_UNUSED(color);
    Q_UNUSED(indices);
    Q_UNUSED(enableDelete);
    Q_UNUSED(state);

    // Store selection and update dropdowns
    QString displayName = name.isEmpty() ? "Selection " + QString::number(id) : name;
    m_selectionsMap[id] = displayName;

    // Clear and refill dropdowns to maintain order
    m_firstSelectionDropdown->clear();
    m_secondSelectionDropdown->clear();

    QMap<int, QString>::const_iterator it;
    for (it = m_selectionsMap.constBegin(); it != m_selectionsMap.constEnd(); ++it) {
        m_firstSelectionDropdown->addItem(it.value(), it.key());
        m_secondSelectionDropdown->addItem(it.value(), it.key());
    }
}

void ComparisonForm::removeSelection(int id) {
    // Remove the selection from map
    if (m_selectionsMap.contains(id)) {
        m_selectionsMap.remove(id);

        // Refresh dropdowns
        m_firstSelectionDropdown->clear();
        m_secondSelectionDropdown->clear();

        QMap<int, QString>::const_iterator it;
        for (it = m_selectionsMap.constBegin(); it != m_selectionsMap.constEnd(); ++it) {
            m_firstSelectionDropdown->addItem(it.value(), it.key());
            m_secondSelectionDropdown->addItem(it.value(), it.key());
        }
    }
}

void ComparisonForm::onSelectionChanged(int index) {
    if (index >= 0) {
        QComboBox* senderComboBox = qobject_cast<QComboBox*>(sender());
        if (senderComboBox) {
            int selectionId = senderComboBox->itemData(index).toInt();
            QString selectionName = senderComboBox->itemText(index);

            if (senderComboBox == m_firstSelectionDropdown) {
                emit firstSelectionChanged(selectionId, selectionName);
            } else if (senderComboBox == m_secondSelectionDropdown) {
                emit secondSelectionChanged(selectionId, selectionName);
            }
        }
    }
}

void ComparisonForm::onCompareToggled(bool checked) {
    if (checked && m_firstSelectionDropdown->count() > 0 && m_secondSelectionDropdown->count() > 0) {
        int firstId = m_firstSelectionDropdown->currentData().toInt();
        int secondId = m_secondSelectionDropdown->currentData().toInt();
        QString firstName = m_firstSelectionDropdown->currentText();
        QString secondName = m_secondSelectionDropdown->currentText();

        emit compareSelections(firstId, secondId, firstName, secondName);
    } else {
        emit compareDisabled();
    }
}

ComparisonForm::~ComparisonForm()
{
    delete ui;
}
