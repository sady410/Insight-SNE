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

public slots:
    void showFeaturesPlot();
    void showDistributionPlot();

private:
    Ui::FeaturesForm *ui;

    QComboBox *m_selectionsDropdown;
    QComboBox *m_featuresDropdown;
};

#endif // FEATURESFORM_H
