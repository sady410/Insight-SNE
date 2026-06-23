#ifndef FEATURESFORM_H
#define FEATURESFORM_H

#include <QWidget>
#include <QMap>
#include <QColor>
#include "barplot.h"
#include "distributionplot.h"
#include <QLayoutItem>

class QComboBox;
class QFrame;

namespace Ui {
class FeaturesForm;
}

class FeaturesForm : public QWidget
{
    Q_OBJECT

public:
    explicit FeaturesForm(QWidget *parent = nullptr);
    ~FeaturesForm();

    void setDataManager(DataManagerPtr dataManager);

    void setFeatureNames(QStringList featureNames);
    void addSelection(int id, const QColor &color, const QList<int>& indices,
                      const bool enableDelete, const bool state, const QString name);
    void removeSelection(int id);

    BarPlot *featuresPlot;
    DistributionPlot *distributionPlot;

signals:
    void selectionChanged(const QString& selectionName);
    void featureChanged(const QString& featureName);

private slots:
    void showFeaturesPlot();
    void showDistributionPlot();
    void onSelectionChanged(int index);
    void onFeatureChanged(int index);
    void handleSelectionChanged(const QString& selectionName);
    void handleFeatureChanged(const QString& featureName);

private:
    Ui::FeaturesForm *ui;
    DataManagerPtr m_dataManager;

    QComboBox *m_selectionsDropdown;
    QComboBox *m_featuresDropdown;
    QMap<int, QString> m_selectionsMap;

    // Styling helper methods
    void applyStyles();
    QFrame* createStyledFrame();
    QFrame* createSeparator();
};

#endif // FEATURESFORM_H
