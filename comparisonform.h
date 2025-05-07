#ifndef COMPARISONFORM_H
#define COMPARISONFORM_H

#include <QWidget>
#include <QComboBox>

namespace Ui {
class ComparisonForm;
}

class ComparisonForm : public QWidget
{
    Q_OBJECT

public:
    explicit ComparisonForm(QWidget *parent = nullptr);
    ~ComparisonForm();

public slots:
    void addSelection(int id, const QColor &color, const QList<int>& indices, const bool enableDelete, const bool state, const QString name);
    void removeSelection(int id);

    void onSelectionChanged(int index);

private:
    Ui::ComparisonForm *ui;
    QMap<int, QString> m_selectionsMap;
    QComboBox *m_firstSelectionDropdown;
    QComboBox *m_secondSelectionDropdown;
};

#endif // COMPARISONFORM_H
