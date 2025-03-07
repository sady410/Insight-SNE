#ifndef MENUFORM_H
#define MENUFORM_H

#include <QWidget>

namespace Ui {
class MenuForm;
}

class MenuForm : public QWidget
{
    Q_OBJECT

public:
    explicit MenuForm(QWidget *parent = nullptr);
    ~MenuForm();

private:
    Ui::MenuForm *ui;
};

#endif // MENUFORM_H
