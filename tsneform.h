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

    TsnePlot *tsnePlot; // TODO MAYBE CREATE GETTER AND PUT MEMBER PRIVATE

private slots:
    void handleSelection(const QColor& color, const QList<int>& selectedIndices);

private:
    Ui::TsneForm *ui;
    QList<QColor> colorList;
};

#endif // TSNEFORM_H
