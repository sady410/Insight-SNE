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

protected:
    void paintEvent(QPaintEvent *event) override {
        qDebug() << "paintEvent called at" << QTime::currentTime().toString("hh:mm:ss.zzz");
        QWidget::paintEvent(event);  // Optional, only needed if base painting logic is relevant
    }

private:
    Ui::TsneForm *ui;

    // New styling methods
    void applyStyles();
    QFrame* createStyledFrame();
    QFrame* createSeparator();
};

#endif // TSNEFORM_H
