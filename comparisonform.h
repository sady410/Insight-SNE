#ifndef COMPARISONFORM_H
#define COMPARISONFORM_H

#include <QWidget>

namespace Ui {
class ComparisonForm;
}

class ComparisonForm : public QWidget
{
    Q_OBJECT

public:
    explicit ComparisonForm(QWidget *parent = nullptr);
    ~ComparisonForm();

private:
    Ui::ComparisonForm *ui;
};

#endif // COMPARISONFORM_H
