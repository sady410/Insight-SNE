#ifndef EXPLANATIONFORM_H
#define EXPLANATIONFORM_H

#include <QWidget>

namespace Ui {
class ExplanationForm;
}

class ExplanationForm : public QWidget
{
    Q_OBJECT

public:
    explicit ExplanationForm(QWidget *parent = nullptr);
    ~ExplanationForm();

private:
    Ui::ExplanationForm *ui;
};

#endif // EXPLANATIONFORM_H
