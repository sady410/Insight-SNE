#ifndef SELECTIONWIDGET_H
#define SELECTIONWIDGET_H

#include <QWidget>
#include <QLabel>

class SelectionWidget : public QWidget {
    Q_OBJECT

public:
    SelectionWidget(int id, QWidget *parent = nullptr);

signals:
    void hovered(int id);
    void left(int id);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void updateLabelStyle();

    int m_id;  // The ID of the selection
};

#endif // SELECTIONWIDGET_H
