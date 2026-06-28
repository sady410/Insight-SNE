#ifndef EXPLANATIONFORM_H
#define EXPLANATIONFORM_H

#include <QWidget>
#include "barplot.h"
#include <QFrame>

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

private:
    Ui::ExplanationForm *ui;

    void applyStyles();
    QFrame* createStyledFrame();
    QFrame* createSeparator();
};

#endif // EXPLANATIONFORM_H
