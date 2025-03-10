#ifndef MENUFORM_H
#define MENUFORM_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QColor>

namespace Ui {
class MenuForm;
}

class MenuForm : public QWidget
{
    Q_OBJECT

public:
    explicit MenuForm(QWidget *parent = nullptr); // Constructor
    ~MenuForm(); // Destructor

signals:
    void requestRemoveSelection(int id);

public slots:
    // Slot to add a new selection to the menu
    void addSelectionToMenu(int id, const QColor &color);

private:
    Ui::MenuForm *ui; // UI object
    QVBoxLayout *verticalLayout; // Horizontal layout to hold selections
};

#endif // MENUFORM_H
