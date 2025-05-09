#ifndef TSNEFORM_H
#define TSNEFORM_H

#include <QWidget>
#include "tsneplot.h"
#include <QFrame>

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

    // New styling methods
    void applyStyles();
    QFrame* createStyledFrame();
    QFrame* createSeparator();
};

#endif // TSNEFORM_H
