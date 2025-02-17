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

    void setGradients(QList<QList<qreal>> *gradients);

private:
    Ui::ExplanationForm *ui;
    ExplanationPlot *explanationPlot;
    QList<QList<qreal>> *gradients;

    QStringList featureNames;
    QStringList labels;

    void setHorizontalBarSeries(QHorizontalBarSeries *horizontalBarSeries);
};

#endif // EXPLANATIONFORM_H
