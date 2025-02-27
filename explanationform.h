#ifndef EXPLANATIONFORM_H
#define EXPLANATIONFORM_H

#include "magnitudesplot.h"
#include "gradientsplot.h"

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

    MagnitudesPlot *magnitudesPlot;
    GradientsPlot *gradientsPlotTop;
    GradientsPlot *gradientsPlotBottom;

public slots:
    void showMagnitudesPlot();
    void showGradientsPlot();

private:
    Ui::ExplanationForm *ui;
};

#endif // EXPLANATIONFORM_H
