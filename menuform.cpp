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

    // Initialize the vertical layout
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setSpacing(10); // Adjust spacing as needed

    // Add a spacer to push content to the top
    m_verticalLayout->addStretch(); // This will act as the spacer

    setLayout(m_verticalLayout);
}

MenuForm::~MenuForm() {
    delete ui;
}

void MenuForm::addSelectionToMenu(int id, const QColor &color, const QList<int>& indices, const bool enableDelete) {
    // Create a container widget for the selection
    SelectionWidget *selectionWidget = new SelectionWidget(id, this);

    // Create a horizontal layout for the selection
    QHBoxLayout *selectionLayout = new QHBoxLayout(selectionWidget);
    selectionLayout->setContentsMargins(0, 0, 0, 0); // Remove margins

    // Add the colored circle
    ColorCircle *circle = new ColorCircle(color, selectionWidget);
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
    m_verticalLayout->insertWidget(0, selectionWidget); // Insert at the top

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
        m_verticalLayout->removeWidget(widget);       // Remove the widget from the layout
        delete widget;                              // Delete the widget
    }
}
