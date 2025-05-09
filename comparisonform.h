#ifndef COMPARISONFORM_H
#define COMPARISONFORM_H

#include <QWidget>
#include <QMap>
#include <QColor>

class QFrame;
class QComboBox;

namespace Ui {
class ComparisonForm;
}

class ComparisonForm : public QWidget
{
    Q_OBJECT

public:
    explicit ComparisonForm(QWidget *parent = nullptr);
    ~ComparisonForm();

    // Methods to manage selections
    void addSelection(int id, const QColor &color, const QList<int>& indices, const bool enableDelete, const bool state, const QString name = QString());
    void removeSelection(int id);

signals:
    void firstSelectionChanged(int id, const QString &name);
    void secondSelectionChanged(int id, const QString &name);
    void compareSelections(int firstId, int secondId, const QString &firstName, const QString &secondName);
    void compareDisabled();

private slots:
    void onSelectionChanged(int index);
    void onCompareToggled(bool checked);

private:
    // UI and styling methods
    void applyStyles();
    QFrame* createStyledFrame();
    QFrame* createSeparator();

    Ui::ComparisonForm *ui;
    QComboBox *m_firstSelectionDropdown;
    QComboBox *m_secondSelectionDropdown;
    QMap<int, QString> m_selectionsMap;
};

#endif // COMPARISONFORM_H
