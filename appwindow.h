#ifndef APPWINDOW_H
#define APPWINDOW_H

#include "selectionmanager.h"

#include <QMainWindow>
#include <QJsonObject>
#include <QResizeEvent>

namespace Ui {
class AppWindow;
}

class AppWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AppWindow(QWidget *parent = nullptr);
    ~AppWindow();

    // Helper methods (used by the builder or other classes)
    QList<qreal> computeMeanData(const QList<QList<qreal>> &dataList);
    QList<QList<qreal>> getAxisGradient(const QList<QList<QList<qreal>>> &gradientsList, int axis);

    // Public getters for UI components
    class TsneForm* getTsneForm() const;
    class MenuForm* getMenuForm() const;
    class OverviewForm* getOverviewForm() const;
    class FeaturesForm* getFeaturesForm() const;
    class ExplanationForm* getExplanationForm() const;

    SelectionManager* getSelectionManager() const;

    void connectSignalsAndSlots();

    void setInitialized(bool initialized) { isInitialized = initialized; }

protected:
    // Override the resizeEvent method
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::AppWindow *ui; // UI components
    SelectionManager *selectionManager;
    bool isInitialized = false; // Flag to track initialization
};

#endif // APPWINDOW_H
