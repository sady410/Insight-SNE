#ifndef TSNEFORM_H
#define TSNEFORM_H

#include "tsneplot.h"

#include <QWidget>
#include <QScatterSeries>
#include <QVBoxLayout>

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
    QList<QColor> colorList;
};

#endif // TSNEFORM_H
