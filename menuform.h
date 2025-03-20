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
    explicit MenuForm(QWidget *parent = nullptr);
    ~MenuForm();

signals:
    void requestRemoveSelection(int id);

    void selectionHovered(int id);
    void selectionLeft(int id);

    void selectionStateChanged(int id);

public slots:
    // Slot to add a new selection to the menu
    void addSelectionToMenu(int id, const QColor &color);
    void removeSelectionWidget(int id);

private:
    Ui::MenuForm *ui;
    QVBoxLayout *verticalLayout; // Vertical layout to hold selections
    QMap<int, QWidget*> selectionWidgets;
};

#endif // MENUFORM_H
