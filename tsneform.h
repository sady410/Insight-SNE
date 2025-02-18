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

public:
    TsnePlot *tsnePlot; // TODO MAYBE CREATE GETTER AND PUT MEMBER PRIVATE

private:
    Ui::TsneForm *ui;
};

#endif // TSNEFORM_H
