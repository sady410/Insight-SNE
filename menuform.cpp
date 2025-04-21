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
#include <QListView>
#include <QLineEdit>
#include <QColorDialog>
#include <QToolTip>
#include <QButtonGroup>
#include <QComboBox>
#include <QRadioButton>

MenuForm::MenuForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MenuForm)
{
    ui->setupUi(this);

    // Main layout setup
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // ===== Top Frame =====
    QFrame *topFrame = new QFrame();
    topFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    topFrame->setLineWidth(2);
    topFrame->setStyleSheet("background-color: #535353;");
    m_top_verticalLayout = new QVBoxLayout(topFrame);
    m_top_verticalLayout->setContentsMargins(8, 8, 8, 8);
    m_top_verticalLayout->setSpacing(6);

    // Title and separator
    QLabel *topTitleLabel = new QLabel("Selections", topFrame);
    topTitleLabel->setAlignment(Qt::AlignCenter);
    topTitleLabel->setStyleSheet("font-weight: bold; color: white; font-size: 16px;");
    m_top_verticalLayout->addWidget(topTitleLabel);

    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("color: #888;");
    m_top_verticalLayout->addWidget(separator);

    // List View setup
    setupListView();

    // Add top frame to layout
    mainLayout->addWidget(topFrame, 4);

    // ===== Bottom Frame =====
    QFrame *bottomFrame = new QFrame();
    bottomFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    bottomFrame->setLineWidth(2);
    bottomFrame->setStyleSheet("background-color: #535353;");
    m_bottomLayout = new QVBoxLayout(bottomFrame);
    m_bottomLayout->setContentsMargins(8, 8, 8, 8);
    m_bottomLayout->setSpacing(6);

    // Title and separator for bottom frame
    QLabel *bottomTitleLabel = new QLabel("Filters", bottomFrame);
    bottomTitleLabel->setAlignment(Qt::AlignCenter);
    bottomTitleLabel->setStyleSheet("font-weight: bold; color: white; font-size: 16px;");
    m_bottomLayout->addWidget(bottomTitleLabel);

    QFrame *bottomSeparator = new QFrame();
    bottomSeparator->setFrameShape(QFrame::HLine);
    bottomSeparator->setFrameShadow(QFrame::Sunken);
    bottomSeparator->setStyleSheet("color: #888;");
    m_bottomLayout->addWidget(bottomSeparator);

    // Button group for mutual exclusivity
    m_filterGroup = new QButtonGroup(this);
    m_filterGroup->setExclusive(false); // Allow toggling

    // List to hold the combo boxes so we can manage their enabled state
    m_comboBoxes.clear();
    m_radioButtons.clear();

    // Track selected index
    m_selectedIndex = new int(-1); // using a pointer to allow lambda capture

    // Add bottom frame to layout
    mainLayout->addWidget(bottomFrame, 2);
}

void MenuForm::setupListView()
{
    // Create list view and model
    m_listView = new QListView(this);
    m_model = new QStandardItemModel(this);

    // Apply model to list view
    m_listView->setModel(m_model);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);

    // Add the list view to the layout
    m_top_verticalLayout->addWidget(m_listView);

    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_listView, &QListView::customContextMenuRequested,
            this, &MenuForm::onListViewContextMenu);
}

void MenuForm::onListViewContextMenu(const QPoint &pos)
{
    QModelIndex index = m_listView->indexAt(pos);
    if (!index.isValid())
        return;

    QStandardItem *item = m_model->itemFromIndex(index);
    if (!item || !item->isCheckable())
        return;

    if (item->checkState() != Qt::Checked) {
        QToolTip::showText(m_listView->viewport()->mapToGlobal(pos),
                           "Enable the checkbox first to change color.");
        return;
    }

    QColor chosenColor = QColorDialog::getColor(Qt::red, this, "Select Feature Color");

    if (chosenColor.isValid()) {
        int selectionId = item->data(Qt::UserRole).toInt();
        QString featureName = item->text();

        if (selectionId != -1) {
            emit arrowsFieldColorChangeRequested(selectionId, featureName, chosenColor);
        }
    }
}

void MenuForm::addSelectionToMenu(int id, const QColor &color, const QList<int> &indices,
                                  const bool enableDelete, const bool state, const QString name)
{
    Q_UNUSED(indices);

    // Create a parent item for the selection
    QStandardItem *selectionItem = new QStandardItem();
    selectionItem->setData(id, Qt::UserRole);
    selectionItem->setData(true, Qt::UserRole + 1); // Mark as header item
    m_model->appendRow(selectionItem);

    // Create the selection widget
    SelectionWidget *selectionWidget = new SelectionWidget(id, this);
    QHBoxLayout *selectionLayout = new QHBoxLayout(selectionWidget);
    selectionLayout->setContentsMargins(0, 0, 0, 0);

    // Always add toggle button (but features will be initially hidden when enableDelete is false)
    QPushButton *toggleButton = new QPushButton(enableDelete ? "-" : "+", selectionWidget);
    toggleButton->setFixedSize(20, 20);
    toggleButton->setStyleSheet(
        "QPushButton {"
        "   border: none;"
        "   font-size: 16px;"
        "   color: white;"
        "   background-color: transparent;"
        "   padding: 0;"
        "}"
        "QPushButton:hover { color: #aaa; }"
        );
    selectionLayout->addWidget(toggleButton);

    // Add color circle
    ColorCircle *circle = new ColorCircle(color, state, selectionWidget);
    selectionLayout->addWidget(circle);

    // Create QLineEdit (editable only if name is empty)
    QLineEdit *lineEdit = new QLineEdit(selectionWidget);
    if (name.isEmpty()) {
        lineEdit->setText("Selection " + QString::number(id));
        lineEdit->setReadOnly(false);
        lineEdit->setStyleSheet(
            "QLineEdit {"
            "   border: 1px solid transparent;"
            "   background: transparent;"
            "   color: white;"
            "}"
            "QLineEdit:hover { border: 1px solid gray; }"
            );
    } else {
        lineEdit->setText(name);
        lineEdit->setReadOnly(true);
        lineEdit->setStyleSheet(
            "QLineEdit {"
            "   border: none;"
            "   background: transparent;"
            "   color: white;"
            "}"
            );
    }
    selectionLayout->addWidget(lineEdit);

    if (enableDelete) {
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

    m_selectionWidgets.insert(id, selectionWidget);
    m_listView->setIndexWidget(m_model->indexFromItem(selectionItem), selectionWidget);

    // Add feature items and track their rows
    QList<int> featureRows;
    for (const QString &featureName : std::as_const(m_featureNames)) {
        QStandardItem *featureItem = new QStandardItem(featureName);
        featureItem->setCheckable(true);
        featureItem->setEditable(false);
        featureItem->setData(id, Qt::UserRole);
        featureItem->setData(false, Qt::UserRole + 1);
        m_model->appendRow(featureItem);
        featureRows.append(m_model->indexFromItem(featureItem).row());

        connect(m_model, &QStandardItemModel::itemChanged, this, [this, id, featureItem](QStandardItem *item) {
            if (item == featureItem) {
                static QHash<QStandardItem*, Qt::CheckState> lastCheckStates;
                Qt::CheckState currentState = item->checkState();
                Qt::CheckState lastState = lastCheckStates.value(item, Qt::Unchecked);

                if (currentState != lastState) {
                    lastCheckStates[item] = currentState;
                    emit featureCheckStateChanged(id, item->text(), currentState == Qt::Checked);
                }
            }
        });
    }

    // Initially hide features if enableDelete is false
    if (!enableDelete) {
        for (int row : featureRows) {
            m_listView->setRowHidden(row, true);
        }
    }

    // Connect toggle button
    connect(toggleButton, &QPushButton::clicked, this, [this, id, toggleButton]() {
        // Find all feature items for this selection
        bool anyVisible = false;
        for (int row = 0; row < m_model->rowCount(); ++row) {
            QStandardItem *item = m_model->item(row);
            if (item && item->data(Qt::UserRole).toInt() == id && !item->data(Qt::UserRole + 1).toBool()) {
                if (!m_listView->isRowHidden(row)) {
                    anyVisible = true;
                    break;
                }
            }
        }

        // Toggle visibility
        for (int row = 0; row < m_model->rowCount(); ++row) {
            QStandardItem *item = m_model->item(row);
            if (item && item->data(Qt::UserRole).toInt() == id && !item->data(Qt::UserRole + 1).toBool()) {
                m_listView->setRowHidden(row, anyVisible);
            }
        }

        // Update button text
        toggleButton->setText(anyVisible ? "+" : "-");
    });

    // Connect other signals
    connect(selectionWidget, &SelectionWidget::hovered, this, &MenuForm::selectionHovered);
    connect(selectionWidget, &SelectionWidget::left, this, &MenuForm::selectionLeft);
    connect(circle, &ColorCircle::clicked, this, [this, id]() {
        emit selectionStateChanged(id);
    });
}

void MenuForm::removeSelectionWidget(int id)
{
    if (m_selectionWidgets.contains(id)) {
        // Find and remove all items associated with this selection
        for (int row = m_model->rowCount() - 1; row >= 0; --row) {
            QStandardItem *item = m_model->item(row);
            if (item && item->data(Qt::UserRole).toInt() == id) {
                m_model->removeRow(row);
            }
        }

        // Remove the widget
        delete m_selectionWidgets.take(id);
    }
}

void MenuForm::addFilterToMenu(int filterIndex, const QString &filterLabel, const QStringList &comboItems)
{
    QHBoxLayout *filterRowLayout = new QHBoxLayout();

    // Create the radio button
    QRadioButton *radioButton = new QRadioButton(filterLabel);
    radioButton->setStyleSheet("color: white;");
    m_filterGroup->addButton(radioButton, filterIndex);
    m_radioButtons.append(radioButton);

    // Create the combo box
    QComboBox *comboBox = new QComboBox();
    comboBox->addItems(comboItems);
    comboBox->setEnabled(false);  // Initially disabled
    m_comboBoxes.append(comboBox);

    // Add widgets to the layout
    filterRowLayout->addWidget(radioButton);
    filterRowLayout->addWidget(comboBox);
    m_bottomLayout->addLayout(filterRowLayout);

    // Handle radio button click behavior
    connect(radioButton, &QRadioButton::clicked, this, [=]() {
        if (*m_selectedIndex == filterIndex) {
            radioButton->setChecked(false);
            comboBox->setEnabled(false);
            *m_selectedIndex = -1;
            emit clearContour();
        } else {
            for (int j = 0; j < m_radioButtons.size(); ++j) {
                m_radioButtons[j]->setChecked(j == filterIndex);
                m_comboBoxes[j]->setEnabled(j == filterIndex);
            }
            *m_selectedIndex = filterIndex;
            emit addContour(comboBox->currentText(), filterLabel);
        }
    });

    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [=](int index) {
                if (radioButton->isChecked()) {
                    emit addContour(comboBox->itemText(index), filterLabel);
                }
            });
}

MenuForm::~MenuForm()
{
    delete ui;
    delete m_selectedIndex;
}
