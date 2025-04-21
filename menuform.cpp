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
#include <QLineEdit>
#include <QColorDialog>
#include <QToolTip>
#include <QButtonGroup>
#include <QComboBox>
#include <QRadioButton>

MenuForm::MenuForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::MenuForm) {
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

    // Tree View setup
    setupTreeView();

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

void MenuForm::addFilterToMenu(int filterIndex, const QString &filterLabel, const QStringList &comboItems) {
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

    // Handle radio button click behavior (toggle enabled state of the combo box)
    connect(radioButton, &QRadioButton::clicked, this, [=]() {
        if (*m_selectedIndex == filterIndex) {
            // If already selected, disable it (toggle off)
            radioButton->setChecked(false);
            comboBox->setEnabled(false);
            *m_selectedIndex = -1; // Reset selection
            emit clearContour(); // Emit clear signal when deselected
        } else {
            // Select new filter
            for (int j = 0; j < m_radioButtons.size(); ++j) {
                m_radioButtons[j]->setChecked(j == filterIndex);
                m_comboBoxes[j]->setEnabled(j == filterIndex);
            }
            *m_selectedIndex = filterIndex;
            // Emit add signal with the filter name and current combo box value
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

    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_treeView, &QTreeView::customContextMenuRequested,
            this, &MenuForm::onTreeViewContextMenu);
}

void MenuForm::onTreeViewContextMenu(const QPoint &pos) {
    QModelIndex index = m_treeView->indexAt(pos);
    if (!index.isValid())
        return;

    QStandardItem *item = m_model->itemFromIndex(index);
    if (!item || !item->isCheckable())
        return;

    if (item->checkState() != Qt::Checked) {
        QToolTip::showText(m_treeView->viewport()->mapToGlobal(pos),
                           "Enable the checkbox first to change color.");
        return;
    }

    QColor chosenColor = QColorDialog::getColor(Qt::red, this, "Select Feature Color");

    if (chosenColor.isValid()) {
        // item->setBackground(chosenColor);

        // Find the parent item to get the selection widget
        QStandardItem *parentItem = item->parent();
        if (parentItem) {
            QModelIndex parentIndex = m_model->indexFromItem(parentItem);
            SelectionWidget *selectionWidget = qobject_cast<SelectionWidget*>(m_treeView->indexWidget(parentIndex));

            if (selectionWidget) {
                // Find the selectionId by looking up the widget in our map
                int selectionId = -1;
                for (auto it = m_selectionWidgets.begin(); it != m_selectionWidgets.end(); ++it) {
                    if (it.value() == selectionWidget) {
                        selectionId = it.key();
                        break;
                    }
                }

                if (selectionId != -1) {
                    QString featureName = item->text();

                    // Emit the signal for color change to the ArrowsManager
                    emit arrowsFieldColorChangeRequested(selectionId, featureName, chosenColor);
                    // TODO to show color in menu form
                    // emit featureColorChanged(item->text(), chosenColor);
                }
            }
        }
    }
}

void MenuForm::addSelectionToMenu(int id, const QColor &color, const QList<int>& indices, const bool enableDelete, const bool state, const QString name) {
    Q_UNUSED(indices);

    SelectionWidget *selectionWidget = new SelectionWidget(id, this);
    QHBoxLayout *selectionLayout = new QHBoxLayout(selectionWidget);
    selectionLayout->setContentsMargins(0, 0, 0, 0);

    ColorCircle *circle = new ColorCircle(color, state, selectionWidget);
    selectionLayout->addWidget(circle);

    // Create QLineEdit (editable only if name is empty)
    QLineEdit *lineEdit = new QLineEdit(selectionWidget);
    if (name.isEmpty()) {
        lineEdit->setText("Selection " + QString::number(id));
        lineEdit->setReadOnly(false);  // Allow editing
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
        lineEdit->setReadOnly(true); // Disable editing
        lineEdit->setStyleSheet(
            "QLineEdit {"
            "   border: none;"
            "   background: transparent;"
            "   color: white;"
            "}"
            );
    }

    selectionLayout->addWidget(lineEdit);

    // Maybe useful at some point
    // if (name.isEmpty()) {
    //     connect(lineEdit, &QLineEdit::editingFinished, this, [this, id, lineEdit]() {
    //         emit selectionNameChanged(id, lineEdit->text());
    //     });
    // }

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

    QStandardItem *selectionItem = new QStandardItem();
    m_model->appendRow(selectionItem);
    m_treeView->setIndexWidget(m_model->indexFromItem(selectionItem), selectionWidget);

    for (const QString &featureName : std::as_const(m_featureNames)) {
        QStandardItem *childItem = new QStandardItem(featureName);
        childItem->setCheckable(true);
        childItem->setEditable(false);
        selectionItem->appendRow(childItem);

        connect(m_model, &QStandardItemModel::itemChanged, this, [this, id, childItem](QStandardItem *item) {
            if (item == childItem) {
                // Check if this was triggered by a check state change
                static QHash<QStandardItem*, Qt::CheckState> lastCheckStates;

                Qt::CheckState currentState = item->checkState();
                Qt::CheckState lastState = lastCheckStates.value(item, Qt::Unchecked);

                if (currentState != lastState) {
                    // The check state has changed, update our stored state
                    lastCheckStates[item] = currentState;
                    qDebug() << "Check state changed";
                    emit featureCheckStateChanged(id, item->text(), currentState == Qt::Checked);
                }
            }
        });
    }

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
