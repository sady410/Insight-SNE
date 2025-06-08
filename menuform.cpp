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
#include <QGraphicsDropShadowEffect>
#include <QStyledItemDelegate>

class SelectionItemDelegate : public QStyledItemDelegate {
public:
    SelectionItemDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        if (index.data(Qt::UserRole + 1).toBool()) {
            painter->save();

            // Draw background matching the style
            painter->fillRect(option.rect, QColor("#363636"));

            // // Draw a subtle separator at the bottom
            // painter->setPen(QPen(QColor("#444444"), 1));
            // QPoint left = option.rect.bottomLeft();
            // QPoint right = option.rect.bottomRight();
            // left.ry() += 0.9;   // Move line 1px down
            // right.ry() += 0.9;
            // painter->drawLine(left, right);

            painter->restore();
            return;
        }

        // Get the standard style for the item
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // Custom painting for feature items (non-header items)
        if (!index.data(Qt::UserRole + 1).toBool()) {
            painter->save();

            // Draw background
            if (opt.state & QStyle::State_Selected) {
                painter->fillRect(opt.rect, QColor("#4CAF50"));
            } else if (opt.state & QStyle::State_MouseOver) {
                painter->fillRect(opt.rect, QColor("#505050"));
            } else if (index.row() % 2 == 1) {
                painter->fillRect(opt.rect, QColor("#383838"));
            } else {
                painter->fillRect(opt.rect, QColor("#363636"));
            }

            // Draw checkbox
            QRect checkRect = QRect(
                opt.rect.left() + 5,
                opt.rect.top() + (opt.rect.height() - 16) / 2,
                16, 16
                );

            painter->setPen(QPen(QColor("#555555"), 1));
            painter->setBrush(QColor("#424242"));
            painter->drawRoundedRect(checkRect, 3, 3);

            if (index.data(Qt::CheckStateRole).toInt() == Qt::Checked) {
                painter->setPen(QPen(Qt::white, 2));
                painter->drawLine(
                    checkRect.left() + 3, checkRect.top() + 8,
                    checkRect.left() + 6, checkRect.top() + 11
                    );
                painter->drawLine(
                    checkRect.left() + 6, checkRect.top() + 11,
                    checkRect.left() + 12, checkRect.top() + 5
                    );
            }

            // Draw text
            if (opt.state & QStyle::State_Selected) {
                painter->setPen(Qt::white);
            } else {
                painter->setPen(Qt::white);
            }

            QFont font = opt.font;
            painter->setFont(font);

            QRect textRect = opt.rect.adjusted(checkRect.width() + 12, 0, -5, 0);
            painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, index.data().toString());

            painter->restore();
        } else {
            // Use default for header items (which will be replaced with SelectionWidget)
            QStyledItemDelegate::paint(painter, option, index);
        }
    }
};

MenuForm::MenuForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MenuForm),
    m_scopeEnabled(false) // Initialize scope state
{
    ui->setupUi(this);

    // Apply the dark theme styling
    applyStyles();

    // Main layout setup
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    this->setLayout(mainLayout);

    // ===== Top Frame =====
    QFrame *topFrame = createStyledFrame();
    m_top_verticalLayout = new QVBoxLayout(topFrame);
    m_top_verticalLayout->setContentsMargins(8, 8, 8, 8);
    m_top_verticalLayout->setSpacing(6);

    // Title and separator
    QLabel *topTitleLabel = new QLabel("Selections", topFrame);
    topTitleLabel->setAlignment(Qt::AlignCenter);
    topTitleLabel->setObjectName("titleLabel");
    topTitleLabel->setStyleSheet("font-weight: bold; color: #4CAF50; font-size: 16px;");
    m_top_verticalLayout->addWidget(topTitleLabel);

    QFrame *separator = createSeparator();
    m_top_verticalLayout->addWidget(separator);

    // List View setup
    setupListView();

    // Add top frame to layout
    mainLayout->addWidget(topFrame, 4);

    // ===== Middle Frame (Scope) =====
    m_scopeFrame = createStyledFrame();
    m_scopeLayout = new QVBoxLayout(m_scopeFrame);
    m_scopeLayout->setContentsMargins(8, 8, 8, 8);
    m_scopeLayout->setSpacing(6);

    // Title and separator for scope frame
    QLabel *scopeTitleLabel = new QLabel("Scope", m_scopeFrame);
    scopeTitleLabel->setAlignment(Qt::AlignCenter);
    scopeTitleLabel->setObjectName("titleLabel");
    scopeTitleLabel->setStyleSheet("font-weight: bold; color: #4CAF50; font-size: 16px;");
    m_scopeLayout->addWidget(scopeTitleLabel);

    QFrame *scopeSeparator = createSeparator();
    m_scopeLayout->addWidget(scopeSeparator);

    // Create container for scope controls
    QWidget *scopeContainer = new QWidget(m_scopeFrame);
    QHBoxLayout *scopeControlLayout = new QHBoxLayout(scopeContainer);
    scopeControlLayout->setSpacing(10);
    scopeControlLayout->setContentsMargins(5, 5, 5, 5);

    // Create status label
    m_scopeLabel = new QLabel("0", scopeContainer);
    m_scopeLabel->setStyleSheet(
        "QLabel {"
        "   color: #CCCCCC;"
        "   font-size: 13px;"
        "   padding: 0px 5px;"
        "   border-radius: 3px;"
        "   background-color: #333333;"
        "   border: 1px solid #444444;"
        "}"
        );
    m_scopeLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_scopeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Create toggle button with improved styling
    m_scopeToggleButton = new QPushButton("Disabled", scopeContainer);
    m_scopeToggleButton->setCheckable(true);
    m_scopeToggleButton->setChecked(false);
    m_scopeToggleButton->setStyleSheet(
        "QPushButton {"
        "   border: 1px solid #555555;"
        "   border-radius: 4px;"
        "   padding: 5px 10px;"
        "   background-color: #424242;"
        "   color: #9E9E9E;"
        "   font-weight: bold;"
        "   min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "   border: 1px solid #4CAF50;"
        "   background-color: #484848;"
        "}"
        "QPushButton:checked {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border: 1px solid #388E3C;"
        "   text-align: center;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #388E3C;"
        "}"
        );

    // Create combobox with styling matching the other comboboxes
    m_scopeComboBox = new QComboBox(scopeContainer);
    m_scopeComboBox->setEnabled(false); // Initially disabled
    m_scopeComboBox->setStyleSheet(
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

    // Add widgets to layout
    scopeControlLayout->addWidget(m_scopeLabel, 2); // Give status label more space
    scopeControlLayout->addWidget(m_scopeToggleButton);
    scopeControlLayout->addWidget(m_scopeComboBox, 1); // Give combobox some space

    // Add container to scope layout
    m_scopeLayout->addWidget(scopeContainer);

    // Add some stretch space to fill the rest of the frame
    m_scopeLayout->addStretch(1);

    // Connect signals
    connect(m_scopeToggleButton, &QPushButton::toggled, this, &MenuForm::onScopeToggled);
    connect(m_scopeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MenuForm::onScopeSelectionChanged);

    // Add scope frame to main layout with a proportional size
    mainLayout->addWidget(m_scopeFrame, 1);

    // ===== Bottom Frame (Filters) =====
    QFrame *bottomFrame = createStyledFrame();
    m_bottomLayout = new QVBoxLayout(bottomFrame);
    m_bottomLayout->setContentsMargins(8, 8, 8, 8);
    m_bottomLayout->setSpacing(6);

    // Title and separator for bottom frame
    QLabel *bottomTitleLabel = new QLabel("Contours", bottomFrame);
    bottomTitleLabel->setAlignment(Qt::AlignCenter);
    bottomTitleLabel->setObjectName("titleLabel");
    bottomTitleLabel->setStyleSheet("font-weight: bold; color: #4CAF50; font-size: 16px;");
    m_bottomLayout->addWidget(bottomTitleLabel);

    QFrame *bottomSeparator = createSeparator();
    m_bottomLayout->addWidget(bottomSeparator);

    // Create a container widget or layout for the filter UI elements
    QWidget *filterContainer = new QWidget(bottomFrame);
    QVBoxLayout *filterLayout = new QVBoxLayout(filterContainer);
    // Add radio buttons, combo boxes, etc. to filterLayout later
    filterLayout->setSpacing(4);
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterContainer->setLayout(filterLayout);

    // Keep a pointer to the filterLayout if needed
    m_filterLayout = filterLayout;

    m_bottomLayout->addWidget(filterContainer);

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

void MenuForm::setDataManager(DataManagerPtr dataManager) {
    m_dataManager = dataManager;
}

void MenuForm::onScopeToggled(bool checked)
{
    m_scopeEnabled = checked;
    m_scopeToggleButton->setText(checked ? "Enabled" : "Disabled");
    m_scopeComboBox->setEnabled(checked);

    if (m_scopeEnabled) {
        emit scopeEnabled(m_scopeLabel->text());
    } else {
        emit scopeDisabled();
    }
}

void MenuForm::onScopeSelectionChanged()
{
    if (m_scopeEnabled) {
        emit scopeEnabled(m_scopeLabel->text());
    }
}

void MenuForm::setScopeInstanceId(int id)
{
    m_scopeLabel->setText(QString::number(id));
    if (m_scopeEnabled) {
        emit scopeEnabled(m_scopeLabel->text());
    }
}

void MenuForm::applyStyles()
{
    // Apply drop shadow to the widget
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 120));
    shadowEffect->setOffset(0, 2);
    this->setGraphicsEffect(shadowEffect);

    // Set the widget style sheet
    this->setStyleSheet(
        "MenuForm {"
        "   background-color: #1d1d1d;"
        "   border-radius: 8px;" // Increased border radius
        "   color: #e6e6e6;"
        "}"
        "QLabel#titleLabel {"
        "   color: #4CAF50;"
        "   font-size: 15px;" // Slightly larger font
        "   font-weight: bold;"
        "   padding: 8px;" // More padding
        "   background-color: #333333;"
        "   border-radius: 4px;"
        "   margin-bottom: 3px;" // Add margin bottom
        "}"
        "QListView {"
        "   background-color: #363636;" // Slightly lighter
        "   border: 1px solid #555555;"
        "   border-radius: 5px;" // Increased radius
        "   alternate-background-color: #383838;"
        "   selection-background-color: #4CAF50;"
        "   selection-color: white;"
        "   padding: 2px;"
        "}"
        "QListView::item {"
        "   padding: 5px;"
        "   border-radius: 3px;"
        "   margin: 1px 0px;"
        "}"
        "QListView::item:hover {"
        "   background-color: #505050;"
        "}"
        "QListView::item:selected {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "}"
        "QRadioButton {"
        "   spacing: 8px;"
        "   color: white;"
        "   margin-bottom: 3px;"
        "   font-size: 13px;"
        "}"
        "QRadioButton::indicator {"
        "   width: 18px;"
        "   height: 18px;"
        "   border-radius: 9px;"
        "}"
        "QRadioButton::indicator:unchecked {"
        "   border: 2px solid #555555;"
        "   background-color: #424242;"
        "}"
        "QRadioButton::indicator:unchecked:hover {"
        "   border: 2px solid #4CAF50;"
        "}"
        "QRadioButton::indicator:checked {"
        "   border: 2px solid #4CAF50;"
        "   background-color: #4CAF50;"
        "   image: url('data:image/svg+xml;utf8,<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"8\" height=\"8\" viewBox=\"0 0 24 24\" fill=\"white\"><circle cx=\"12\" cy=\"12\" r=\"6\"/></svg>');"
        "   background-position: center;"
        "   background-repeat: no-repeat;"
        "}"
        "QComboBox {"
        "   border: 1px solid #555555;"
        "   border-radius: 5px;"
        "   padding: 6px 20px 6px 12px;" // Increased padding
        "   min-width: 8em;"
        "   background-color: #424242;"
        "   min-height: 28px;" // Taller
        "   color: white;"
        "   font-size: 13px;"
        "}"
        "QComboBox:hover {"
        "   border: 1px solid #4CAF50;"
        "   background-color: #484848;" // Slight highlight on hover
        "}"
        "QComboBox:disabled {"
        "   background-color: #323232;"
        "   color: #9E9E9E;"
        "}"
        "QComboBox::drop-down {"
        "   subcontrol-origin: padding;"
        "   subcontrol-position: top right;"
        "   width: 22px;" // Slightly wider
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
        "QScrollBar:vertical {"
        "   border: none;"
        "   background-color: #2d2d2d;"
        "   width: 12px;" // Slightly wider
        "   margin: 0px;"
        "   border-radius: 6px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background-color: #4CAF50;"
        "   min-height: 35px;" // Larger minimum handle
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

QFrame* MenuForm::createStyledFrame()
{
    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    frame->setLineWidth(2);
    frame->setStyleSheet(
        "QFrame {"
        "   background-color: #2d2d2d;"
        "   border-radius: 8px;" // Increased border radius
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

QFrame* MenuForm::createSeparator()
{
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet(
        "color: #555555; "
        "background-color: #555555; "
        "margin: 5px 0px;" // Add some vertical margin
        );
    return separator;
}

void MenuForm::setupListView()
{
    // Create list view and model
    m_listView = new QListView(this);
    m_model = new QStandardItemModel(this);

    m_listView->setStyleSheet(
        "QListView {"
        "   background-color: #363636;"
        "   border: 1px solid #555555;"
        "   border-radius: 5px;"
        "   padding: 0px;" // Remove padding to align items properly
        "}"
        "QListView::item {"
        "   padding: 0px;" // Remove item padding
        "   margin: 0px;"  // Remove margins
        "   height: 40px;" // Fixed height for items
        "}"
        "QListView::item:hover {"
        "   background-color: #505050;"
        "}"
        "QListView::item:selected {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "}"
        );

    m_listView->setUniformItemSizes(true);
    // Apply model to list view
    m_listView->setModel(m_model);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listView->setAlternatingRowColors(true);
    m_listView->setItemDelegate(new SelectionItemDelegate(this)); // Custom item delegate

    // Add the list view to the layout
    m_top_verticalLayout->addWidget(m_listView);

    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_listView, &QListView::customContextMenuRequested,
            this, &MenuForm::onListViewContextMenu);
}

void MenuForm::connectModelSignals()
{
    connect(m_model, &QStandardItemModel::itemChanged, this,
            [this](QStandardItem *item) {
                if (item && item->isCheckable() && !item->data(Qt::UserRole + 1).toBool()) {
                    int id = item->data(Qt::UserRole).toInt();
                    emit featureCheckStateChanged(id, item->text(), item->checkState() == Qt::Checked);
                }
            });
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

void MenuForm::uncheckAllFeaturesForSelection(int selectionId)
{
    // Temporarily disconnect the itemChanged signal
    disconnect(m_model, &QStandardItemModel::itemChanged, this, nullptr);

    // Uncheck all features
    for (int row = 0; row < m_model->rowCount(); ++row) {
        QStandardItem *item = m_model->item(row);
        if (item &&
            item->data(Qt::UserRole).toInt() == selectionId &&
            !item->data(Qt::UserRole + 1).toBool())
        {
            item->setCheckState(Qt::Unchecked);
        }
    }

    // Reconnect the itemChanged signal
    connect(m_model, &QStandardItemModel::itemChanged, this,
            [this](QStandardItem *item) {
                if (item && item->isCheckable() && !item->data(Qt::UserRole + 1).toBool()) {
                    int id = item->data(Qt::UserRole).toInt();
                    emit featureCheckStateChanged(id, item->text(), item->checkState() == Qt::Checked);
                }
            }
            );

    // Emit state changes
    emitFeatureStateChangesForSelection(selectionId, false);
}

void MenuForm::emitFeatureStateChangesForSelection(int selectionId, bool state)
{
    for (int row = 0; row < m_model->rowCount(); ++row) {
        QStandardItem *item = m_model->item(row);
        if (item &&
            item->data(Qt::UserRole).toInt() == selectionId &&
            !item->data(Qt::UserRole + 1).toBool())
        {
            if ((state && item->checkState() != Qt::Checked) ||
                (!state && item->checkState() != Qt::Unchecked)) {
                item->setCheckState(state ? Qt::Checked : Qt::Unchecked);
            }
            emit featureCheckStateChanged(selectionId, item->text(), state);
        }
    }
}

void MenuForm::setCheckboxesEnabledForSelection(int selectionId, bool enabled)
{
    for (int row = 0; row < m_model->rowCount(); ++row) {
        QStandardItem *item = m_model->item(row);
        if (item &&
            item->data(Qt::UserRole).toInt() == selectionId &&
            !item->data(Qt::UserRole + 1).toBool())  // Not a header item
        {
            if (enabled) {
                item->setFlags(item->flags() | Qt::ItemIsEnabled);
            } else {
                item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
                // Optionally uncheck when disabling
                item->setCheckState(Qt::Unchecked);
            }
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
    selectionItem->setSizeHint(QSize(-1, 40)); // Match widget height
    m_model->appendRow(selectionItem);

    // Create the selection widget with proper size constraints
    SelectionWidget *selectionWidget = new SelectionWidget(id, this);
    selectionWidget->setMinimumHeight(40);
    selectionWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *selectionLayout = new QHBoxLayout(selectionWidget);
    selectionLayout->setContentsMargins(8, 6, 8, 6); // Tighter, consistent margins
    selectionLayout->setSpacing(10);

    // Toggle button (expand/collapse)
    QPushButton *toggleButton = new QPushButton(enableDelete ? "-" : "+", selectionWidget);
    toggleButton->setFixedSize(24, 16);
    toggleButton->setStyleSheet(
        "QPushButton {"
        "   border: 1px solid #555555;"
        "   border-radius: 12px;"
        "   font-weight: bold;"
        "   font-size: 16px;"
        "   color: white;"
        "   background-color: #424242;"
        "   padding: 0;"
        "   margin: 1px;"
        "   text-align: center;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4CAF50;"
        "   border: 1px solid #388E3C;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #2E7D32;"
        "}"
        );

    // Color circle
    ColorCircle *circle = new ColorCircle(color, state, selectionWidget);
    circle->setFixedSize(28, 28);

    // Name field
    QLineEdit *lineEdit = new QLineEdit(selectionWidget);
    if (name.isEmpty()) {
        lineEdit->setText("Selection " + QString::number(id));
        lineEdit->setReadOnly(false);
        lineEdit->setStyleSheet(
            "QLineEdit {"
            "   border: 1px solid transparent;"
            "   border-radius: 3px;"
            "   background: transparent;"
            "   color: white;"
            "   font-weight: bold;"
            "   font-size: 13px;"
            "   padding: 2px 5px;"
            "}"
            "QLineEdit:hover { border: 1px solid #4CAF50; }"
            "QLineEdit:focus { border: 1px solid #4CAF50; background-color: rgba(76, 175, 80, 0.15); }"
            );
    } else {
        lineEdit->setText(name);
        lineEdit->setReadOnly(true);
        lineEdit->setStyleSheet(
            "QLineEdit {"
            "   border: none;"
            "   background: transparent;"
            "   color: white;"
            "   font-weight: bold;"
            "   font-size: 13px;"
            "   padding: 2px 5px;"
            "}"
            );
    }

    // Point count label
    QLabel *pointCountLabel = new QLabel(QString::number(indices.size()) + " pts", selectionWidget);
    pointCountLabel->setStyleSheet(
        "QLabel {"
        "   color: #AAAAAA;"
        "   font-size: 12px;"
        "   background-color: rgba(255, 255, 255, 0.05);"
        "   border-radius: 6px;"
        "   padding: 2px 6px;"
        "   margin-left: 8px;"
        "}"
        );
    pointCountLabel->setAlignment(Qt::AlignVCenter);

    // Optional delete button
    QPushButton *deleteButton = nullptr;
    if (enableDelete) {
        deleteButton = new QPushButton("×", selectionWidget);
        deleteButton->setFixedSize(24, 16);
        deleteButton->setStyleSheet(
            "QPushButton {"
            "   border: 1px solid #555555;"
            "   border-radius: 12px;"
            "   font-weight: bold;"
            "   font-size: 16px;"
            "   color: #CCCCCC;"
            "   background-color: transparent;"
            "   padding: 0;"
            "   margin: 1px;"
            "   text-align: center;"
            "}"
            "QPushButton:hover {"
            "   color: #FF5252;"
            "   background-color: rgba(255, 82, 82, 0.1);"
            "}"
            "QPushButton:pressed {"
            "   color: #FF1744;"
            "   background-color: rgba(255, 82, 82, 0.2);"
            "}"
            );

        connect(deleteButton, &QPushButton::clicked, this, [this, id]() {
            uncheckAllFeaturesForSelection(id);
            emit requestRemoveSelection(id);
            removeSelectionWidget(id);
        });
    }

    // Assemble layout
    selectionLayout->addWidget(circle);
    selectionLayout->addWidget(lineEdit);
    selectionLayout->addWidget(pointCountLabel);

    // Right-aligned button area
    QSpacerItem *spacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    selectionLayout->addItem(spacer);
    selectionLayout->addWidget(toggleButton);
    if (deleteButton)
        selectionLayout->addWidget(deleteButton);

    // Apply hover styling
    selectionWidget->setStyleSheet(
        "SelectionWidget {"
        "   border-radius: 4px;"
        "   padding: 2px;"
        "   background-color: transparent;"
        "}"
        "SelectionWidget:hover {"
        "   background-color: rgba(76, 175, 80, 0.1);"
        "}"
        );

    m_selectionWidgets.insert(id, selectionWidget);
    m_listView->setIndexWidget(m_model->indexFromItem(selectionItem), selectionWidget);

    // Rest of the function remains the same...
    // Add feature items and track their rows
    QList<int> featureRows;
    for (const QString &featureName : std::as_const(m_dataManager->featureNames())) {
        QStandardItem *featureItem = new QStandardItem(featureName);
        featureItem->setSizeHint(QSize(-1, 30));
        featureItem->setCheckable(true);
        featureItem->setEditable(false);
        featureItem->setData(id, Qt::UserRole);
        featureItem->setData(false, Qt::UserRole + 1);

        // Style the checkboxes with CSS
        featureItem->setData(
            "QStandardItem::indicator {"
            "   width: 16px; height: 16px;"
            "   border: 1px solid #555555;"
            "   background-color: #424242;"
            "   border-radius: 3px;"
            "}"
            "QStandardItem::indicator:checked {"
            "   background-image: url('data:image/svg+xml;utf8,<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"12\" height=\"12\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"white\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><polyline points=\"20 6 9 17 4 12\"></polyline></svg>');"
            "   background-position: center;"
            "   background-repeat: no-repeat;"
            "   background-color: #4CAF50;"
            "   border: 1px solid #4CAF50;"
            "}",
            Qt::UserRole + 2
            );

        m_model->appendRow(featureItem);
        featureRows.append(m_model->indexFromItem(featureItem).row());
    }

    bool initiallyEnabled = circle->isActive();
    for (int row : featureRows) {
        QStandardItem *item = m_model->item(row);
        if (!initiallyEnabled) {
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        }
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
    connect(circle, &ColorCircle::clicked, this, [this, id, circle]() {
        emit selectionStateChanged(id);

        bool isActive = circle->isActive();
        setCheckboxesEnabledForSelection(id, isActive);

        if (!isActive) {
            uncheckAllFeaturesForSelection(id);
        }
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
    radioButton->setStyleSheet(
        "QRadioButton {"
        "   color: white;"
        "   font-size: 12px;"
        "   margin-bottom: 2px;"
        "}"
        "QRadioButton::indicator {"
        "   width: 16px;"
        "   height: 16px;"
        "   border-radius: 8px;"
        "}"
        "QRadioButton::indicator:unchecked {"
        "   border: 2px solid #555555;"
        "   background-color: #424242;"
        "}"
        "QRadioButton::indicator:unchecked:hover {"
        "   border: 2px solid #4CAF50;"
        "}"
        "QRadioButton::indicator:checked {"
        "   border: 2px solid #4CAF50;"
        "   background-color: #4CAF50;"
        "}"
        );

    m_filterGroup->addButton(radioButton, filterIndex);
    m_radioButtons.append(radioButton);

    // Create the combo box
    QComboBox *comboBox = new QComboBox();
    comboBox->addItems(comboItems);
    comboBox->setEnabled(false);  // Initially disabled
    comboBox->setStyleSheet(
        "QComboBox {"
        "   border: 1px solid #555555;"
        "   border-radius: 4px;"
        "   padding: 5px 18px 5px 10px;"
        "   min-width: 6em;"
        "   background-color: #424242;"
        "   min-height: 25px;"
        "   color: white;"
        "}"
        "QComboBox:hover {"
        "   border: 1px solid #4CAF50;"
        "}"
        "QComboBox:disabled {"
        "   background-color: #323232;"
        "   color: #9E9E9E;"
        "}"
        "QComboBox::drop-down {"
        "   subcontrol-origin: padding;"
        "   subcontrol-position: top right;"
        "   width: 20px;"
        "   border-left-width: 1px;"
        "   border-left-color: #555555;"
        "   border-left-style: solid;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #424242;"
        "   border: 1px solid #555555;"
        "   selection-background-color: #4CAF50;"
        "   selection-color: white;"
        "}"
        );
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
