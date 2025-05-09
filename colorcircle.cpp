#include "colorcircle.h"

ColorCircle::ColorCircle(const QColor &color, const bool state, QWidget *parent)
    : QWidget(parent), m_color(color), m_active(state)
{
    setFixedSize(28, 28);
    setCursor(Qt::PointingHandCursor);

    // Add animation properties
    m_animation = new QPropertyAnimation(this, "hoverValue");
    m_animation->setDuration(150);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    m_hoverValue = 0;
}

void ColorCircle::setActive(bool active) {
    if (m_active != active) {
        m_active = active;
        update();
    }
}

void ColorCircle::setColor(const QColor &color) {
    if (m_color != color) {
        m_color = color;
        update();
    }
}

void ColorCircle::setHoverValue(qreal value) {
    m_hoverValue = value;
    update();
}

void ColorCircle::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const int padding = 2;
    QRect circleRect = rect().adjusted(padding, padding, -padding, -padding);

    // Draw outer ring (always visible)
    QPen pen;
    pen.setWidth(2);

    if (m_active) {
        pen.setColor(m_color);
    } else {
        pen.setColor(QColor(100, 100, 100));
    }

    painter.setPen(pen);

    // Fill circle
    QBrush brush;
    if (m_active) {
        // Active state - use the color with some transparency for a gradient effect
        QRadialGradient gradient(circleRect.center(), circleRect.width() / 2);
        gradient.setColorAt(0, m_color.lighter(115));
        gradient.setColorAt(1, m_color);
        brush = gradient;
    } else {
        // Inactive state - gray gradient
        QRadialGradient gradient(circleRect.center(), circleRect.width() / 2);
        gradient.setColorAt(0, QColor(80, 80, 80));
        gradient.setColorAt(1, QColor(60, 60, 60));
        brush = gradient;
    }

    painter.setBrush(brush);
    painter.drawEllipse(circleRect);

    // Draw hover/pulse effect
    if (m_hoverValue > 0) {
        painter.setOpacity(m_hoverValue * 0.3); // Max opacity 30%
        painter.setPen(Qt::NoPen);

        // Use white for glow effect
        painter.setBrush(QColor(255, 255, 255, 100));

        // Draw slightly larger circle for glow
        int glowSize = m_hoverValue * 5; // Up to 5 pixels larger
        QRect glowRect = circleRect.adjusted(-glowSize, -glowSize, glowSize, glowSize);
        painter.drawEllipse(glowRect);
    }

    // Draw checkmark if active
    if (m_active) {
        painter.setOpacity(1.0);
        painter.setPen(QPen(Qt::white, 2));

        // Simple checkmark
        int centerX = width() / 2;
        int centerY = height() / 2;

        QPointF points[3] = {
            QPointF(centerX - 5, centerY),
            QPointF(centerX - 2, centerY + 3),
            QPointF(centerX + 5, centerY - 4)
        };

        painter.drawPolyline(points, 3);
    }
}

void ColorCircle::enterEvent(QEnterEvent *event)
{
    m_animation->stop();
    m_animation->setStartValue(m_hoverValue);
    m_animation->setEndValue(1.0);
    m_animation->start();
    QWidget::enterEvent(event);
}

void ColorCircle::leaveEvent(QEvent *event)
{
    m_animation->stop();
    m_animation->setStartValue(m_hoverValue);
    m_animation->setEndValue(0.0);
    m_animation->start();
    QWidget::leaveEvent(event);
}

void ColorCircle::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_active = !m_active;
        update();
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}
