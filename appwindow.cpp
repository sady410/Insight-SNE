#include "appwindow.h"
#include "arrowsmanager.h"
#include "ui_appwindow.h"

AppWindow::AppWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AppWindow)
    , m_selectionManager(new SelectionManager(this)), m_arrowsManager(new ArrowsManager(this))
{
    ui->setupUi(this);
    qDebug() << "AppWindow UI initialized.";
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
    return ui->menuForm;
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

    // Zoom
    connect(getTsneForm()->tsnePlot, &TsnePlot::zoomChanged, getOverviewForm()->overviewPlot, &OverviewPlot::updateZoomRectangle);
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
