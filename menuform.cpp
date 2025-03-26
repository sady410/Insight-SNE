#include "menuform.h"
#include "ui_menuform.h"
#include "selectionwidget.h"
#include "colorcircle.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QColor>
#include <QPainter>
#include <QStyleOption>


MenuForm::MenuForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::MenuForm) {
    ui->setupUi(this);

    // Main layout setup (unchanged)
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Top section
    QFrame *topFrame = new QFrame();
    topFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    topFrame->setLineWidth(2);
    topFrame->setStyleSheet("background-color: #495759;");
    m_top_verticalLayout = new QVBoxLayout(topFrame);
    m_top_verticalLayout->setContentsMargins(8, 8, 8, 8);
    m_top_verticalLayout->setSpacing(6);

    // 1. FIRST add the title and separator
    QLabel *topTitleLabel = new QLabel("Selections", topFrame);
    topTitleLabel->setAlignment(Qt::AlignCenter);
    topTitleLabel->setStyleSheet("font-weight: bold; color: white; font-size: 16px;");
    m_top_verticalLayout->addWidget(topTitleLabel);

    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("color: #888;");
    m_top_verticalLayout->addWidget(separator);

    // Bottom section
    QFrame *bottomFrame = new QFrame();
    bottomFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    bottomFrame->setLineWidth(2);
    bottomFrame->setStyleSheet("background-color: #495759;");
    m_bottom_verticalLayout = new QVBoxLayout(bottomFrame);
    m_bottom_verticalLayout->setContentsMargins(8, 8, 8, 8);
    m_bottom_verticalLayout->setSpacing(6);

    QLabel *bottomTitleLabel = new QLabel("Settings", topFrame);
    bottomTitleLabel->setAlignment(Qt::AlignCenter);
    bottomTitleLabel->setStyleSheet("font-weight: bold; color: white; font-size: 16px;");
    m_bottom_verticalLayout->addWidget(bottomTitleLabel);

    QFrame *separator2 = new QFrame();
    separator2->setFrameShape(QFrame::HLine);
    separator2->setFrameShadow(QFrame::Sunken);
    separator2->setStyleSheet("color: #888;");
    m_bottom_verticalLayout->addWidget(separator2);

    QLabel *label = new QLabel("TODO...overlays, etc...");
    m_bottom_verticalLayout->layout()->addWidget(label);

    m_bottom_verticalLayout->addStretch();

    mainLayout->addWidget(topFrame, 1);
    mainLayout->addWidget(bottomFrame, 1);

    // Optional: Set object names for CSS targeting if needed
    // topFrame->setObjectName("topFrame");
    // bottomFrame->setObjectName("bottomFrame");
}

MenuForm::~MenuForm() {
    delete ui;
}

void MenuForm::addSelectionToMenu(int id, const QColor &color, const QList<int>& indices, const bool enableDelete, const bool state) {
    Q_UNUSED(indices);
    // Create a container widget for the selection
    SelectionWidget *selectionWidget = new SelectionWidget(id, this);

    // Create a horizontal layout for the selection
    QHBoxLayout *selectionLayout = new QHBoxLayout(selectionWidget);
    selectionLayout->setContentsMargins(0, 0, 0, 0); // Remove margins

    // Add the colored circle
    ColorCircle *circle = new ColorCircle(color, state, selectionWidget);
    selectionLayout->addWidget(circle);

    // Add the label
    QLabel *label = new QLabel("Selection " + QString::number(id), selectionWidget);
    selectionLayout->addWidget(label);

    if (enableDelete) {
        // Add the delete button (cross)
        QPushButton *deleteButton = new QPushButton("×", selectionWidget);
        deleteButton->setFixedSize(25, 25); // Larger button size
        deleteButton->setStyleSheet(
            "QPushButton { "
            "   border: none; "
            "   font-size: 30px; "
            "   color: white; "
            "   background-color: transparent; "
            "}"
            "QPushButton:hover { "
            "   color: darkred; "
            "}"
            );
        selectionLayout->addWidget(deleteButton);

        // Connect the delete button to a slot for deletion
        connect(deleteButton, &QPushButton::clicked, this, [this, id]() {
            emit requestRemoveSelection(id); // Emit signal to remove the selection
            removeSelectionWidget(id);      // Remove the widget from the UI
        });

    }

    // Insert the selection widget at the top of the main layout
   // m_top_verticalLayout->addWidget(selectionWidget);

    m_top_verticalLayout->insertWidget(2, selectionWidget); // Position 2 = after title+separator

    // Ensure stretch exists at the bottom
    if (!m_hasStretch) {
        m_top_verticalLayout->addStretch();
        m_hasStretch = true;
    }

    // Store the selection widget in the map
    m_selectionWidgets.insert(id, selectionWidget);

    // Connect the hover signals from the selectionWidget to the MenuForm signals
    connect(selectionWidget, &SelectionWidget::hovered, this, &MenuForm::selectionHovered);
    connect(selectionWidget, &SelectionWidget::left, this, &MenuForm::selectionLeft);

    connect(circle, &ColorCircle::clicked, this, [this, id]() {
        emit selectionStateChanged(id);
    });
}

void MenuForm::removeSelectionWidget(int id) {
    if (m_selectionWidgets.contains(id)) {
        QWidget *widget = m_selectionWidgets.take(id); // Remove the widget from the map
        m_top_verticalLayout->removeWidget(widget);       // Remove the widget from the layout
        delete widget;                              // Delete the widget
    }
}
