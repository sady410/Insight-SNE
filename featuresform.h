#ifndef FEATURESFORM_H
#define FEATURESFORM_H

#include "barplot.h"

#include <QWidget>

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

private:
    Ui::FeaturesForm *ui;
};

#endif // FEATURESFORM_H
