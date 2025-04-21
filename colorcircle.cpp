#include "colorcircle.h"

ColorCircle::ColorCircle(const QColor &color, const bool state, QWidget *parent)
    : QWidget(parent), m_color(color), m_active(state) {
    setFixedSize(15, 15); // Small circle size
}

void ColorCircle::toggleState() {
    m_active = !m_active;
    repaint(); // Force repaint
}

bool ColorCircle::isActive() const {
    return m_active;
}

void ColorCircle::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_active) {
        painter.setBrush(m_color); // Fill the circle with color
    } else {
        painter.setBrush(Qt::NoBrush); // No fill, only border
    }

    painter.setPen(QPen(Qt::black, 1)); // Border color and thickness
    painter.drawEllipse(rect());
}

void ColorCircle::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        toggleState();
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}
