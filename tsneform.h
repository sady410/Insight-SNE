#ifndef TSNEFORM_H
#define TSNEFORM_H

#include "tsneplot.h"

#include <QWidget>

namespace Ui {
class TsneForm;
}

class TsneForm : public QWidget
{
    Q_OBJECT

public:
    explicit TsneForm(QWidget *parent = nullptr);
    ~TsneForm();
    TsnePlot *tsnePlot;

private:
    Ui::TsneForm *ui;
};

#endif // TSNEFORM_H
