#ifndef MENUFORM_H
#define MENUFORM_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QColor>
#include <QListView>
#include <QStandardItem>
#include <QRadioButton>
#include <QComboBox>

namespace Ui {
class MenuForm;
}

class MenuForm : public QWidget
{
    Q_OBJECT

public:
    explicit MenuForm(QWidget *parent = nullptr);
    ~MenuForm();

    void setFeatureNames(QStringList featureNames) { m_featureNames = featureNames; }
    void addScopeToCombobox(QStringList name) { m_scopeComboBox->addItems(name); }
    void connectModelSignals();

signals:
    void requestRemoveSelection(int id);

    void selectionHovered(int id);
    void selectionLeft(int id);

    void selectionStateChanged(int id);

    void featureCheckStateChanged(int id, const QString &featureName, bool checked);

    void selectionNameChanged(int id, const QString& newName);

    void featureColorChanged(const QString &featureName, const QColor &color);
    void arrowsFieldColorChangeRequested(int selectionId, const QString &featureName, const QColor &newColor);

    void addContour(const QString &comboValue, const QString &filterName);
    void clearContour();

    void scopeEnabled(const QString &id); // scope is features or global
    void scopeDisabled();

public slots:
    void addSelectionToMenu(int id, const QColor &color,
                            const QList<int> &indices, const bool enableDelete,
                            const bool state, const QString name);
    void addFilterToMenu(int filterIndex, const QString &filterLabel, const QStringList &comboItems);
    void removeSelectionWidget(int id);
    void setScopeInstanceId(int pointId);

private slots:
    void onListViewContextMenu(const QPoint &pos);
    void onScopeSelectionChanged();
    void onScopeToggled(bool checked);

private:
    Ui::MenuForm *ui;
    bool m_hasStretch = false;
    QVBoxLayout *m_top_verticalLayout; // Vertical layout to hold selections
    QVBoxLayout *m_bottom_verticalLayout;
    QMap<int, QWidget*> m_selectionWidgets;

    QButtonGroup *m_filterGroup;
    QList<QRadioButton*> m_radioButtons;
    QList<QComboBox*> m_comboBoxes;
    int *m_selectedIndex;
    QVBoxLayout *m_bottomLayout;
    QVBoxLayout *m_filterLayout;

    QStringList m_featureNames;

    QListView *m_listView;
    QStandardItemModel *m_model;

    QFrame *m_scopeFrame;
    QVBoxLayout *m_scopeLayout;
    QComboBox *m_scopeComboBox;
    QPushButton *m_scopeToggleButton;
    QLabel *m_scopeLabel;
    bool m_scopeEnabled;

    void setCheckboxesEnabledForSelection(int selectionId, bool enabled);
    void uncheckAllFeaturesForSelection(int selectionId);
    void emitFeatureStateChangesForSelection(int selectionId, bool state);
    void setupListView();

    void applyStyles();
    QFrame* createStyledFrame();
    QFrame* createSeparator();
};

#endif // MENUFORM_H
