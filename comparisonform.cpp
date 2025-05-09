#include "comparisonform.h"
#include "ui_comparisonform.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
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
    frame->setStyleSheet("background-color: #d1d1d1;");

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
    titleLabel->setStyleSheet("font-weight: bold; color: black; font-size: 16px;");
    headerLayout->addWidget(titleLabel);

    QFrame *separator = new QFrame(headerWidget);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("color: #000000;");
    headerLayout->addWidget(separator);

    verticalLayout->addWidget(headerWidget);

    // Add spacing between header and controls
    verticalLayout->addSpacing(15);

    // Add horizontal layout with radio button and two dropdowns separated by minus symbol
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(10);

    QCheckBox *radioButton = new QCheckBox("Toggle", frame);
    radioButton->setStyleSheet("color: white;");
    radioButton->setChecked(false);
    controlsLayout->addWidget(radioButton);

    m_firstSelectionDropdown = new QComboBox(frame);
    m_firstSelectionDropdown->addItem("Selection 1");
    m_firstSelectionDropdown->addItem("Selection 2");
    m_firstSelectionDropdown->addItem("Selection 3");
    controlsLayout->addWidget(m_firstSelectionDropdown);

    QLabel *minusLabel = new QLabel("====| DIFF |====", frame);
    minusLabel->setAlignment(Qt::AlignCenter);
    minusLabel->setStyleSheet("color: black; font-weight: bold;");
    controlsLayout->addWidget(minusLabel);

    m_secondSelectionDropdown = new QComboBox(frame);
    m_secondSelectionDropdown->addItem("Selection 1");
    m_secondSelectionDropdown->addItem("Selection 2");
    m_secondSelectionDropdown->addItem("Selection 3");
    controlsLayout->addWidget(m_secondSelectionDropdown);

    verticalLayout->addLayout(controlsLayout);

    // Add stretch to push everything to the top
    verticalLayout->addStretch();

    mainLayout->addWidget(frame);

    // Connect dropdown signals
    connect(m_firstSelectionDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ComparisonForm::onSelectionChanged);
    // Connect dropdown signals
    connect(m_secondSelectionDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ComparisonForm::onSelectionChanged);
}

void ComparisonForm::addSelection(int id, const QColor &color, const QList<int>& indices, const bool enableDelete, const bool state, const QString name) {
    // Mark unused parameters
    Q_UNUSED(color);
    Q_UNUSED(indices);
    Q_UNUSED(enableDelete);
    Q_UNUSED(state);

    m_selectionsMap[id] = name;
    m_firstSelectionDropdown->addItem(name);
    m_secondSelectionDropdown->addItem(name);
}

void ComparisonForm::removeSelection(int id) {
    // Remove the selection from dropdown and map
    if (m_selectionsMap.contains(id)) {
        int firstIndex = m_firstSelectionDropdown->findText(m_selectionsMap[id]);
        int secondIndex = m_secondSelectionDropdown->findText(m_selectionsMap[id]);
        if (firstIndex >= 0 && secondIndex >= 0) {
            m_firstSelectionDropdown->removeItem(firstIndex);
            m_secondSelectionDropdown->removeItem(secondIndex);
        }
        m_selectionsMap.remove(id);
    }
}

void ComparisonForm::onSelectionChanged(int index) {
    // if (index >= 0) {
    //     QString selectionName = m_selectionsDropdown->itemText(index);
    //     emit selectionChanged(selectionName);
    // }
}

ComparisonForm::~ComparisonForm()
{
    delete ui;
}
