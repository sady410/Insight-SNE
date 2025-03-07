#ifndef EXPLANATIONFORM_H
#define EXPLANATIONFORM_H

#include "barplot.h"

#include <QWidget>
#include <QHorizontalBarSeries>

namespace Ui {
class ExplanationForm;
}

class ExplanationForm : public QWidget
{
    Q_OBJECT

public:
    explicit ExplanationForm(QWidget *parent = nullptr);
    ~ExplanationForm();

    BarPlot *magnitudesPlot;
    BarPlot *gradientsPlotTop;
    BarPlot *gradientsPlotBottom;

public slots:
    void showMagnitudesPlot();
    void showGradientsPlot();

private:
    Ui::ExplanationForm *ui;
};

#endif // EXPLANATIONFORM_H
