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
#include <QDebug>
#include <QStandardItemModel>
#include <QTreeView>

MenuForm::MenuForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::MenuForm) {
    ui->setupUi(this);

    // Main layout setup
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

    // add the title and separator
    QLabel *topTitleLabel = new QLabel("Selections", topFrame);
    topTitleLabel->setAlignment(Qt::AlignCenter);
    topTitleLabel->setStyleSheet("font-weight: bold; color: white; font-size: 16px;");
    m_top_verticalLayout->addWidget(topTitleLabel);

    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("color: #888;");
    m_top_verticalLayout->addWidget(separator);

    // Initialize Tree View
    setupTreeView();

    mainLayout->addWidget(topFrame, 1);
}

void MenuForm::setupTreeView() {
    // Create tree view and model
    m_treeView = new QTreeView(this);
    m_model = new QStandardItemModel(this);

    // Set column title
    m_model->setColumnCount(1);
    m_model->setHeaderData(0, Qt::Horizontal, "Available selections");

    // Apply model to tree view
    m_treeView->setModel(m_model);
    m_treeView->setRootIsDecorated(true);
    m_treeView->setHeaderHidden(false);

    // Add the tree view to the layout
    m_top_verticalLayout->addWidget(m_treeView);
}

void MenuForm::addSelectionToMenu(int id, const QColor &color, const QList<int>& indices, bool enableDelete, bool state) {
    Q_UNUSED(indices);

    // Create a new SelectionWidget
    SelectionWidget *selectionWidget = new SelectionWidget(id, this);
    QHBoxLayout *selectionLayout = new QHBoxLayout(selectionWidget);
    selectionLayout->setContentsMargins(0, 0, 0, 0);

    // Add the colored circle
    ColorCircle *circle = new ColorCircle(color, state, selectionWidget);
    selectionLayout->addWidget(circle);

    // Add the label
    QLabel *label = new QLabel("Selection " + QString::number(id), selectionWidget);
    selectionLayout->addWidget(label);

    if (enableDelete) {
        // Add the delete button (cross)
        QPushButton *deleteButton = new QPushButton("×", selectionWidget);
        deleteButton->setFixedSize(25, 25);
        deleteButton->setStyleSheet(
            "QPushButton { border: none; font-size: 30px; color: white; background-color: transparent; }"
            "QPushButton:hover { color: darkred; }"
            );
        selectionLayout->addWidget(deleteButton);

        connect(deleteButton, &QPushButton::clicked, this, [this, id]() {
            emit requestRemoveSelection(id);
            removeSelectionWidget(id);
        });
    }

    // Store the selection widget
    m_selectionWidgets.insert(id, selectionWidget);

    // Add an item to the model
    QStandardItem *selectionItem = new QStandardItem();
    m_model->appendRow(selectionItem);

    // Set the SelectionWidget as the actual widget for the item
    m_treeView->setIndexWidget(m_model->indexFromItem(selectionItem), selectionWidget);

    for (const QString &featureName : std::as_const(m_featureNames)) {
        QStandardItem *childItem = new QStandardItem(featureName);
        childItem->setCheckable(true);
        selectionItem->appendRow(childItem);

        // Connect directly to the item's state change
        connect(m_model, &QStandardItemModel::itemChanged, this, [this, id, childItem](QStandardItem *item) {
            if (item == childItem) { // Ensure signal is for the correct item
                emit featureCheckStateChanged(id, item->text(), item->checkState() == Qt::Checked);
            }
        });
    }

    // Expand default behaviour
    // treeView->expandAll();
    // treeView->expand(model->indexFromItem(selectionItem));

    // Connect the hover signals from the selectionWidget to the MenuForm signals
    connect(selectionWidget, &SelectionWidget::hovered, this, &MenuForm::selectionHovered);
    connect(selectionWidget, &SelectionWidget::left, this, &MenuForm::selectionLeft);

    connect(circle, &ColorCircle::clicked, this, [this, id]() {
        emit selectionStateChanged(id);
    });
}

void MenuForm::removeSelectionWidget(int id) {
    if (m_selectionWidgets.contains(id)) {
        QWidget *widget = m_selectionWidgets.take(id);
        delete widget;
    }

    for (int row = 0; row < m_model->rowCount(); ++row) {
        QStandardItem *item = m_model->item(row);
        if (item && m_treeView->indexWidget(m_model->index(row, 0)) == m_selectionWidgets.value(id)) {
            m_model->removeRow(row);
            return;
        }
    }
}

MenuForm::~MenuForm() {
    delete ui;
}
