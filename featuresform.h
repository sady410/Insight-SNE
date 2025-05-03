#ifndef FEATURESFORM_H
#define FEATURESFORM_H

#include "barplot.h"
#include "distributionplot.h"
#include <QWidget>

class QComboBox;

namespace Ui {
class FeaturesForm;
}

class FeaturesForm : public QWidget
{
    Q_OBJECT

public:
    explicit FeaturesForm(QWidget *parent = nullptr);
    ~FeaturesForm();

    BarPlot *featuresPlot;
    DistributionPlot *distributionPlot;

    void setFeatureNames(QStringList featureNames);

public slots:
    void showFeaturesPlot();
    void showDistributionPlot();
    void addSelection(int id, const QColor &color, const QList<int>& indices, const bool enableDelete, const bool state, const QString name);
    void removeSelection(int id);

    // New slots for dropdown changes
    void onSelectionChanged(int index);
    void onFeatureChanged(int index);

    // Handler methods to update distribution plot
    void handleSelectionChanged(const QString& selectionName);
    void handleFeatureChanged(const QString& featureName);

signals:
    // New signals to emit when dropdowns change
    void selectionChanged(const QString& selectionName);
    void featureChanged(const QString& featureName);

private:
    Ui::FeaturesForm *ui;
    QMap<int, QString> m_selectionsMap;
    QComboBox *m_selectionsDropdown;
    QComboBox *m_featuresDropdown;
};

#endif // FEATURESFORM_H
