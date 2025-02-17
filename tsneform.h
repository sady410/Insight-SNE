#ifndef TSNEFORM_H
#define TSNEFORM_H

#include "tsneplot.h"

#include <QWidget>
#include <QScatterSeries>

namespace Ui {
class TsneForm;
}

class TsneForm : public QWidget
{
    Q_OBJECT

public:
    explicit TsneForm(QWidget *parent = nullptr);
    ~TsneForm();

    void setScatterSeries(QScatterSeries *scatterSeries);

private:
    Ui::TsneForm *ui;
    TsnePlot *tsnePlot;
    QScatterSeries *data;
};

#endif // TSNEFORM_H
