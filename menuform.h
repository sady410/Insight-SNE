#ifndef MENUFORM_H
#define MENUFORM_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QColor>
#include <QTreeView>
#include <QStandardItem>

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
    void addSelectionToMenu(int id, const QColor &color, const QList<int>& indices, const bool enableDelete, const bool state);
    void removeSelectionWidget(int id);

private:
    Ui::MenuForm *ui;
    bool m_hasStretch = false;
    QVBoxLayout *m_top_verticalLayout; // Vertical layout to hold selections
    QVBoxLayout *m_bottom_verticalLayout;
    QMap<int, QWidget*> m_selectionWidgets;

    QTreeView *treeView;
    QStandardItemModel *model;

    void setupTreeView();
};

#endif // MENUFORM_H
