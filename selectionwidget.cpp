#include "selectionwidget.h"

SelectionWidget::SelectionWidget(int id, QWidget *parent)
    : QWidget(parent), m_id(id) {}

void SelectionWidget::enterEvent(QEnterEvent *event) {
    QWidget::enterEvent(event);
    emit hovered(m_id);  // Emit the hovered signal
}

void SelectionWidget::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
    emit left(m_id);  // Emit the left signal
}
