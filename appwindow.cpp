#include "appwindow.h"
#include "arrowsmanager.h"
#include "menuwindow.h"
#include "ui_appwindow.h"
#include <QStyleFactory>
#include <QGraphicsDropShadowEffect>
#include <QScreen>

AppWindow::AppWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AppWindow)
    , m_selectionManager(new SelectionManager(this)), m_arrowsManager(new ArrowsManager(this))
{
    ui->setupUi(this);
    qDebug() << "AppWindow UI initialized.";

    // Set application style and UI customizations
    applyStylesheet();
    setupUi();

    ui->comparisonForm->hide(); // TODO

    // Center the window on the screen
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);

    // Set window title
    this->setWindowTitle("Insight-SNE Visualization");
}

void AppWindow::setupUi()
{
    // Add drop shadows to various containers
    QList<QWidget*> formWidgets = {
        ui->tsneForm, ui->menuForm_2, ui->overviewForm,
        ui->featuresForm, ui->explanationForm, ui->comparisonForm
    };

    for (QWidget* widget : formWidgets) {
        QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect;
        shadowEffect->setBlurRadius(10);
        shadowEffect->setColor(QColor(0, 0, 0, 100));
        shadowEffect->setOffset(0, 1);
        widget->setGraphicsEffect(shadowEffect);

        // Apply a frame style to each widget
        widget->setStyleSheet("background-color: #2d2d2d; border-radius: 5px; margin: 2px;");
    }

    // Adjust the layout spacing for better appearance
    ui->horizontalLayout_2->setSpacing(8);
    ui->verticalLayout->setSpacing(8);
    ui->horizontalLayout_4->setSpacing(8);
    ui->horizontalLayout->setSpacing(8);
    ui->verticalLayout_2->setSpacing(8);

    // Add margins
    ui->horizontalLayout_2->setContentsMargins(8, 8, 8, 8);
}

void AppWindow::applyStylesheet()
{
    // Set application style to Fusion
    qApp->setStyle(QStyleFactory::create("Fusion"));

    // Create a palette for the window
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::WindowText, QColor(230, 230, 230));
    darkPalette.setColor(QPalette::Base, QColor(60, 60, 60));
    darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(230, 230, 230));
    darkPalette.setColor(QPalette::ToolTipText, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::Text, QColor(230, 230, 230));
    darkPalette.setColor(QPalette::Button, QColor(60, 60, 60));
    darkPalette.setColor(QPalette::ButtonText, QColor(230, 230, 230));
    darkPalette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

    // Apply the palette
    qApp->setPalette(darkPalette);

    // Set style sheet for specific widgets
    QString styleSheet = R"(
        QMainWindow {
            background-color: #1d1d1d;
        }

        QWidget {
            color: #e6e6e6;
        }

        /* Form titles */
        QLabel[objectName^="titleLabel"] {
            color: #4CAF50;
            font-size: 14px;
            font-weight: bold;
            padding: 5px;
            background-color: #333333;
            border-radius: 3px;
        }

        /* Plot styling */
        QCustomPlot {
            background-color: #424242;
            border: 1px solid #555555;
            border-radius: 3px;
        }

        /* Button styling */
        QPushButton {
            background-color: #424242;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 5px;
            color: white;
            min-height: 25px;
        }

        QPushButton:hover {
            background-color: #4CAF50;
            border: 1px solid #388E3C;
        }

        QPushButton:disabled {
            background-color: #323232;
            color: #9E9E9E;
        }

        /* Slider styling */
        QSlider::groove:horizontal {
            height: 10px;
            background: #424242;
            border-radius: 5px;
        }

        QSlider::handle:horizontal {
            background: #4CAF50;
            width: 20px;
            height: 20px;
            margin: -5px 0;
            border-radius: 10px;
        }

        QSlider::handle:horizontal:hover {
            background: #66BB6A;
        }

        QSlider::sub-page:horizontal {
            background: #2E7D32;
            border-radius: 5px;
        }

        QSlider::add-page:horizontal {
            background: #424242;
            border-radius: 5px;
        }

        /* Checkbox styling */
        QCheckBox {
            spacing: 8px;
            min-height: 25px;
        }

        QCheckBox::indicator {
            width: 20px;
            height: 20px;
        }

        QCheckBox::indicator:unchecked {
            border: 2px solid #555555;
            background-color: #424242;
            border-radius: 3px;
        }

        QCheckBox::indicator:unchecked:hover {
            border: 2px solid #4CAF50;
        }

        QCheckBox::indicator:checked {
            border: 2px solid #4CAF50;
            background-color: #4CAF50;
            border-radius: 3px;
        }

        QCheckBox::indicator:checked:hover {
            background-color: #66BB6A;
            border: 2px solid #66BB6A;
        }

        /* Combobox styling */
        QComboBox {
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 5px 18px 5px 10px;
            min-width: 6em;
            background-color: #424242;
            min-height: 25px;
        }

        QComboBox:hover {
            border: 1px solid #4CAF50;
        }

        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border-left-width: 1px;
            border-left-color: #555555;
            border-left-style: solid;
        }

        QComboBox QAbstractItemView {
            background-color: #424242;
            border: 1px solid #555555;
            selection-background-color: #4CAF50;
            selection-color: white;
        }

        /* Scrollbar styling */
        QScrollBar:vertical {
            border: none;
            background-color: #2d2d2d;
            width: 10px;
            margin: 0px;
        }

        QScrollBar::handle:vertical {
            background-color: #4CAF50;
            min-height: 30px;
            border-radius: 5px;
        }

        QScrollBar::handle:vertical:hover {
            background-color: #66BB6A;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }

        QScrollBar:horizontal {
            border: none;
            background-color: #2d2d2d;
            height: 10px;
            margin: 0px;
        }

        QScrollBar::handle:horizontal {
            background-color: #4CAF50;
            min-width: 30px;
            border-radius: 5px;
        }

        QScrollBar::handle:horizontal:hover {
            background-color: #66BB6A;
        }

        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }

        /* Menu styling */
        QMenuBar {
            background-color: #333333;
            color: #e6e6e6;
        }

        QMenuBar::item {
            background: transparent;
        }

        QMenuBar::item:selected {
            background: #4CAF50;
        }

        QMenu {
            background-color: #333333;
            border: 1px solid #555555;
        }

        QMenu::item {
            padding: 5px 30px 5px 30px;
        }

        QMenu::item:selected {
            background-color: #4CAF50;
        }

        /* Status bar styling */
        QStatusBar {
            background-color: #333333;
            color: #e6e6e6;
        }
    )";

    this->setStyleSheet(styleSheet);
}

class TsneForm* AppWindow::getTsneForm() const {
    return ui->tsneForm;
}

class OverviewForm* AppWindow::getOverviewForm() const {
    return ui->overviewForm;
}

class FeaturesForm* AppWindow::getFeaturesForm() const {
    return ui->featuresForm;
}

class ExplanationForm* AppWindow::getExplanationForm() const {
    return ui->explanationForm;
}

class MenuForm* AppWindow::getMenuForm() const {
    return ui->menuForm_2;
}

SelectionManager* AppWindow::getSelectionManager() const {
    return m_selectionManager;
}

void AppWindow::connectSignalsAndSlots() { // TODO REFACTOR FOR BARPLOTS

    // Selection creation
    connect(getTsneForm()->tsnePlot, &TsnePlot::pointsSelected, m_selectionManager,
            static_cast<void (SelectionManager::*)(const QList<int>&)>(&SelectionManager::createSelection));

    connect(m_selectionManager, &SelectionManager::selectionCreated, getMenuForm(), &MenuForm::addSelectionToMenu);

    connect(m_selectionManager, &SelectionManager::selectionCreated, getExplanationForm()->magnitudesPlot, &BarPlot::addValue);
    connect(m_selectionManager, &SelectionManager::selectionCreated, getExplanationForm()->gradientsPlotTop, &BarPlot::addValue);
    connect(m_selectionManager, &SelectionManager::selectionCreated, getExplanationForm()->gradientsPlotBottom, &BarPlot::addValue);

    connect(m_selectionManager, &SelectionManager::selectionCreated, getFeaturesForm()->featuresPlot, &BarPlot::addValue);
    connect(m_selectionManager, &SelectionManager::selectionCreated, getFeaturesForm()->distributionPlot, &DistributionPlot::addSelection);
    connect(m_selectionManager, &SelectionManager::selectionCreated, getFeaturesForm(), &FeaturesForm::addSelection);

    // Click on point in tsne plot
    connect(getTsneForm()->tsnePlot, &TsnePlot::pointClicked,getMenuForm(), &MenuForm::setScopeInstanceId);

    // Selection removal
    connect(getMenuForm(), &MenuForm::requestRemoveSelection, m_selectionManager, &SelectionManager::removeSelection);

    connect(m_selectionManager, &SelectionManager::selectionRemoved, getFeaturesForm(), &FeaturesForm::removeSelection);
    connect(m_selectionManager, &SelectionManager::selectionRemoved, getFeaturesForm()->featuresPlot, &BarPlot::removeValue);
    connect(m_selectionManager, &SelectionManager::selectionRemoved, getFeaturesForm()->distributionPlot, &DistributionPlot::removeSelection);

    connect(m_selectionManager, &SelectionManager::selectionRemoved, getExplanationForm()->magnitudesPlot, &BarPlot::removeValue);
    connect(m_selectionManager, &SelectionManager::selectionRemoved, getExplanationForm()->gradientsPlotTop, &BarPlot::removeValue);
    connect(m_selectionManager, &SelectionManager::selectionRemoved, getExplanationForm()->gradientsPlotBottom, &BarPlot::removeValue);
    connect(m_selectionManager, &SelectionManager::selectionRemoved, this, [this](int) {
        ui->tsneForm->tsnePlot->decreaseSelectionCount();
    });

    // Hovering
    connect(getMenuForm(), &MenuForm::selectionHovered, m_selectionManager, &SelectionManager::findHoveredSelectionAndEmitSignal);
    connect(getMenuForm(), &MenuForm::selectionLeft, m_selectionManager, &SelectionManager::stopHighlightSelection);

    connect(m_selectionManager, &SelectionManager::startHighlightSelection, getTsneForm()->tsnePlot, &TsnePlot::startHighlightSelection);
    connect(m_selectionManager, &SelectionManager::stopHighlightSelection, getTsneForm()->tsnePlot, &TsnePlot::stopHighlightSelection);

    connect(m_selectionManager, &SelectionManager::startHighlightSelection, getFeaturesForm()->featuresPlot, &BarPlot::startHighlightSelection);
    connect(m_selectionManager, &SelectionManager::stopHighlightSelection, getFeaturesForm()->featuresPlot, &BarPlot::stopHighlightSelection);

    connect(m_selectionManager, &SelectionManager::startHighlightSelection, getExplanationForm()->magnitudesPlot, &BarPlot::startHighlightSelection);
    connect(m_selectionManager, &SelectionManager::stopHighlightSelection, getExplanationForm()->magnitudesPlot, &BarPlot::stopHighlightSelection);

    connect(m_selectionManager, &SelectionManager::startHighlightSelection, getExplanationForm()->gradientsPlotTop, &BarPlot::startHighlightSelection);
    connect(m_selectionManager, &SelectionManager::stopHighlightSelection, getExplanationForm()->gradientsPlotTop, &BarPlot::stopHighlightSelection);

    connect(m_selectionManager, &SelectionManager::startHighlightSelection, getExplanationForm()->gradientsPlotBottom, &BarPlot::startHighlightSelection);
    connect(m_selectionManager, &SelectionManager::stopHighlightSelection, getExplanationForm()->gradientsPlotBottom, &BarPlot::stopHighlightSelection);

    // Enabling/Disabling selections
    connect(getMenuForm(), &MenuForm::selectionStateChanged, m_selectionManager, &SelectionManager::changeSelectionStateAndEmitSignal);

    connect(m_selectionManager, &SelectionManager::selectionEnabled, getFeaturesForm()->featuresPlot, &BarPlot::addValue);
    connect(m_selectionManager, &SelectionManager::selectionDisabled, getFeaturesForm()->featuresPlot, &BarPlot::removeValue);

    connect(m_selectionManager, &SelectionManager::selectionEnabled, getExplanationForm()->magnitudesPlot, &BarPlot::addValue);
    connect(m_selectionManager, &SelectionManager::selectionDisabled, getExplanationForm()->magnitudesPlot, &BarPlot::removeValue);

    connect(m_selectionManager, &SelectionManager::selectionEnabled, getExplanationForm()->gradientsPlotTop, &BarPlot::addValue);
    connect(m_selectionManager, &SelectionManager::selectionDisabled, getExplanationForm()->gradientsPlotTop, &BarPlot::removeValue);

    connect(m_selectionManager, &SelectionManager::selectionEnabled, getExplanationForm()->gradientsPlotBottom, &BarPlot::addValue);
    connect(m_selectionManager, &SelectionManager::selectionDisabled, getExplanationForm()->gradientsPlotBottom, &BarPlot::removeValue);

    // Click on bar in explanations
    connect(getExplanationForm()->gradientsPlotBottom, &BarPlot::barClicked, m_selectionManager, &SelectionManager::selectionClickedInBarPlot);
    connect(getExplanationForm()->gradientsPlotTop, &BarPlot::barClicked, m_selectionManager, &SelectionManager::selectionClickedInBarPlot);
    connect(getExplanationForm()->magnitudesPlot, &BarPlot::barClicked, m_selectionManager, &SelectionManager::selectionClickedInBarPlot);

    // Feature arrow checkbox enable/disable
    connect(getMenuForm(), &MenuForm::featureCheckStateChanged, m_selectionManager, &SelectionManager::selectionFreatureArrowCheckboxChanged);
    connect(m_selectionManager, &SelectionManager::addArrows, m_arrowsManager, &ArrowsManager::createArrowsField);
    connect(m_selectionManager, &SelectionManager::removeArrows, m_arrowsManager, &ArrowsManager::removeArrowsField);
    connect(m_arrowsManager, &ArrowsManager::arrowsFieldCreated, getTsneForm()->tsnePlot, &TsnePlot::addArrows);
    connect(m_arrowsManager, &ArrowsManager::arrowsFieldRemoved, getTsneForm()->tsnePlot, &TsnePlot::removeArrows);

    // Feature arrow change color
    connect(getMenuForm(), &MenuForm::arrowsFieldColorChangeRequested, m_arrowsManager, &ArrowsManager::changeArrowsFieldColor);
    connect(m_arrowsManager, &ArrowsManager::arrowsFieldColorChanged, getTsneForm()->tsnePlot, &TsnePlot::changeArrowsColor);

    // Contours
    connect(getMenuForm(), &MenuForm::addContour, getTsneForm()->tsnePlot, &TsnePlot::drawContourPlot);
    connect(getMenuForm(), &MenuForm::clearContour, getTsneForm()->tsnePlot, &TsnePlot::clearContourPlot);

    // Scope enable/disable
    connect(getMenuForm(), &MenuForm::scopeEnabled, getTsneForm()->tsnePlot, &TsnePlot::drawScopeContourPlot);
    connect(getMenuForm(), &MenuForm::scopeDisabled, getTsneForm()->tsnePlot, &TsnePlot::clearContourPlot);

    // Zoom
    connect(getTsneForm()->tsnePlot, &TsnePlot::zoomChanged, getOverviewForm()->overviewPlot, &OverviewPlot::updateZoomRectangle);

    // Connect button to go back to menu
    connect(getOverviewForm()->m_backButton, &QPushButton::clicked, this, &AppWindow::returnToMenu);
}

void AppWindow::returnToMenu()
{
    // Create the menu window - adjust the class name as needed for your application
    MenuWindow* menuWindow = new MenuWindow();

    // Show the menu window
    menuWindow->show();

    // Close the AppWindow
    this->close();
}

AppWindow::~AppWindow() {
    delete ui;
}

QList<qreal> AppWindow::computeMeanData(const QList<QList<qreal>> &dataList) {
    QList<qreal> meanDataList;

    if (dataList.isEmpty()) {
        return meanDataList; // Return empty list if input is empty
    }

    // Determine the number of columns (assuming all rows have the same number of columns)
    int numColumns = dataList[0].size();

    // Initialize the meanDataList with zeros
    meanDataList.resize(numColumns);
    std::fill(meanDataList.begin(), meanDataList.end(), 0.0);

    // Iterate through each row and accumulate column values
    for (const QList<qreal> &row : dataList) {
        if (row.size() != numColumns) {
            qWarning() << "Row size mismatch! Skipping row.";
            continue;
        }

        for (int col = 0; col < numColumns; ++col) {
            meanDataList[col] += row[col];
        }
    }

    // Divide by the number of rows to compute the mean for each column
    int numRows = dataList.size();
    for (int col = 0; col < numColumns; ++col) {
        meanDataList[col] /= numRows;
    }

    return meanDataList;
}

QList<QList<qreal>> AppWindow::getAxisGradient(const QList<QList<QList<qreal>>> &gradientsList, int axis) {
    QList<QList<qreal>> result;

    for (const QList<QList<qreal>> &gradient : gradientsList) {
        if (axis >= 0 && axis < gradient.size()) {
            result.append(gradient[axis]);
        } else {
            qWarning() << "Axis out of bounds!";
            return QList<QList<qreal>>();
        }
    }

    return result;
}

void AppWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    if (!m_isInitialized) {
        return;
    }

    // Get current axis ranges from QCustomPlot
    TsnePlot* tsnePlot = ui->tsneForm->tsnePlot;
    if (tsnePlot) {
        QCPRange xRange = tsnePlot->xAxis->range();
        QCPRange yRange = tsnePlot->yAxis->range();
        emit tsnePlot->zoomChanged(xRange.lower, xRange.upper, yRange.lower, yRange.upper);
    }
}
