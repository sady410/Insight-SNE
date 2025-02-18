#ifndef OVERVIEWFORM_H
#define OVERVIEWFORM_H

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

private:
    Ui::OverviewForm *ui;
};

#endif // OVERVIEWFORM_H
