#ifndef OVERVIEWFORM_H
#define OVERVIEWFORM_H

#include "overviewplot.h"

#include <QWidget>

namespace Ui {
class OverviewForm;
}

class OverviewForm : public QWidget
{
    Q_OBJECT

public:
    explicit OverviewForm(QWidget *parent = nullptr);
    ~OverviewForm();
    OverviewPlot *overviewPlot;

    void setDatasetInfo(const QString &info);
    QPushButton *m_backButton;

private:
    Ui::OverviewForm *ui;

    QFrame* createSeparator();
    QFrame* createStyledFrame();
    void applyStyles();

    QLabel *m_infoLabel;
};

#endif // OVERVIEWFORM_H
