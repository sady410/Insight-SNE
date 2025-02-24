#ifndef EXPLANATIONFORM_H
#define EXPLANATIONFORM_H

#include "explanationplot.h"

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

    ExplanationPlot *explanationPlot;

private:
    Ui::ExplanationForm *ui;

    void changeActivePlot();
};

#endif // EXPLANATIONFORM_H
