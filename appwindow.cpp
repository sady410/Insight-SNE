#include "appwindow.h"
#include "ui_appwindow.h"

#include <QValueAxis>

AppWindow::AppWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AppWindow)
    , selectionManager(new SelectionManager(this))
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

void AppWindow::connectSignalsAndSlots() { // TODO REFACTOR FOR BARPLOTS

    // Selection creation
    connect(getTsneForm()->tsnePlot, &TsnePlot::pointsSelected, selectionManager, &SelectionManager::createSelection);

    connect(selectionManager, &SelectionManager::selectionCreated, getMenuForm(), &MenuForm::addSelectionToMenu);
    connect(selectionManager, &SelectionManager::selectionCreated, getExplanationForm()->magnitudesPlot, &BarPlot::addValue);
    connect(selectionManager, &SelectionManager::selectionCreated, getExplanationForm()->gradientsPlotTop, &BarPlot::addValue);
    connect(selectionManager, &SelectionManager::selectionCreated, getExplanationForm()->gradientsPlotBottom, &BarPlot::addValue);
    connect(selectionManager, &SelectionManager::selectionCreated, getFeaturesForm()->featuresPlot, &BarPlot::addValue);

    // Selection removal
    connect(getMenuForm(), &MenuForm::requestRemoveSelection, selectionManager, &SelectionManager::removeSelection);

    connect(selectionManager, &SelectionManager::selectionRemoved, getFeaturesForm()->featuresPlot, &BarPlot::removeValue);
    connect(selectionManager, &SelectionManager::selectionRemoved, getExplanationForm()->magnitudesPlot, &BarPlot::removeValue);
    connect(selectionManager, &SelectionManager::selectionRemoved, getExplanationForm()->gradientsPlotTop, &BarPlot::removeValue);
    connect(selectionManager, &SelectionManager::selectionRemoved, getExplanationForm()->gradientsPlotBottom, &BarPlot::removeValue);
    connect(selectionManager, &SelectionManager::selectionRemoved, this, [this](int) {
        ui->tsneForm->tsnePlot->decreaseSelectionCount();
    });

    // Hovering
    connect(getMenuForm(), &MenuForm::selectionHovered, selectionManager, &SelectionManager::findHoveredSelectionAndEmitSignal);
    connect(getMenuForm(), &MenuForm::selectionLeft, selectionManager, &SelectionManager::stopHighlightSelection);

    connect(selectionManager, &SelectionManager::startHighlightSelection, getTsneForm()->tsnePlot, &TsnePlot::startHighlightSelection);
    connect(selectionManager, &SelectionManager::stopHighlightSelection, getTsneForm()->tsnePlot, &TsnePlot::stopHighlightSelection);

    connect(selectionManager, &SelectionManager::startHighlightSelection, getFeaturesForm()->featuresPlot, &BarPlot::startHighlightSelection);
    connect(selectionManager, &SelectionManager::stopHighlightSelection, getFeaturesForm()->featuresPlot, &BarPlot::stopHighlightSelection);

    connect(selectionManager, &SelectionManager::startHighlightSelection, getExplanationForm()->magnitudesPlot, &BarPlot::startHighlightSelection);
    connect(selectionManager, &SelectionManager::stopHighlightSelection, getExplanationForm()->magnitudesPlot, &BarPlot::stopHighlightSelection);

    connect(selectionManager, &SelectionManager::startHighlightSelection, getExplanationForm()->gradientsPlotTop, &BarPlot::startHighlightSelection);
    connect(selectionManager, &SelectionManager::stopHighlightSelection, getExplanationForm()->gradientsPlotTop, &BarPlot::stopHighlightSelection);

    connect(selectionManager, &SelectionManager::startHighlightSelection, getExplanationForm()->gradientsPlotBottom, &BarPlot::startHighlightSelection);
    connect(selectionManager, &SelectionManager::stopHighlightSelection, getExplanationForm()->gradientsPlotBottom, &BarPlot::stopHighlightSelection);

    // Enabling/Disabling selections
    connect(getMenuForm(), &MenuForm::selectionStateChanged, selectionManager, &SelectionManager::changeSelectionStateAndEmitSignal);

    connect(selectionManager, &SelectionManager::selectionEnabled, getFeaturesForm()->featuresPlot, &BarPlot::addValue);
    connect(selectionManager, &SelectionManager::selectionDisabled, getFeaturesForm()->featuresPlot, &BarPlot::removeValue);

    connect(selectionManager, &SelectionManager::selectionEnabled, getExplanationForm()->magnitudesPlot, &BarPlot::addValue);
    connect(selectionManager, &SelectionManager::selectionDisabled, getExplanationForm()->magnitudesPlot, &BarPlot::removeValue);

    connect(selectionManager, &SelectionManager::selectionEnabled, getExplanationForm()->gradientsPlotTop, &BarPlot::addValue);
    connect(selectionManager, &SelectionManager::selectionDisabled, getExplanationForm()->gradientsPlotTop, &BarPlot::removeValue);

    connect(selectionManager, &SelectionManager::selectionEnabled, getExplanationForm()->gradientsPlotBottom, &BarPlot::addValue);
    connect(selectionManager, &SelectionManager::selectionDisabled, getExplanationForm()->gradientsPlotBottom, &BarPlot::removeValue);

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

    if (!isInitialized) {
        return;
    }

    // Force OverviewPlot to update zoom rectangle
    QValueAxis *axisX = qobject_cast<QValueAxis *>(ui->tsneForm->tsnePlot->chart()->axes(Qt::Horizontal).first());
    QValueAxis *axisY = qobject_cast<QValueAxis *>(ui->tsneForm->tsnePlot->chart()->axes(Qt::Vertical).first());

    if (axisX && axisY) {
        emit ui->tsneForm->tsnePlot->zoomChanged(axisX->min(), axisX->max(), axisY->min(), axisY->max());
    }
}
