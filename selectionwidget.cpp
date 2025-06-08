#include "selectionwidget.h"

#include <QPainter>
#include <QDebug>
#include <QStyle>
#include <QStyleOption>

SelectionWidget::SelectionWidget(int id, QWidget *parent)
    : QWidget(parent), m_id(id), m_hoverOpacity(0.0)
{
    // Set minimum height for consistent appearance
    setMinimumHeight(42);

    // Enable mouse tracking so we can track hover state
    setMouseTracking(true);

    // Important to ensure the widget accepts the layout from parent
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Make sure the widget paints its own background
    setAttribute(Qt::WA_StyledBackground, true);

    // Setup hover animation for smooth transitions
    m_hoverAnimation = new QPropertyAnimation(this, "hoverOpacity");
    m_hoverAnimation->setDuration(200);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void SelectionWidget::setHoverOpacity(qreal opacity)
{
    if (m_hoverOpacity != opacity) {
        m_hoverOpacity = opacity;
        update();
    }
}

void SelectionWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. First draw the base background
    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    // 2. Draw hover effect (if active)
    if (m_hoverOpacity > 0.0) {
        QColor hoverColor = QColor(76, 175, 80, static_cast<int>(40 * m_hoverOpacity));
        painter.setBrush(hoverColor);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect(), 6, 6);
    }

    // 3. Draw pressed effect (if active)
    if (m_pressed) {
        painter.setBrush(QColor(0, 0, 0, 40));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect(), 6, 6);
    }

    // 4. Draw border if needed (optional)
    if (hasFocus()) {
        painter.setPen(QPen(QColor(76, 175, 80), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 6, 6);
    }
}

void SelectionWidget::enterEvent(QEnterEvent *event)
{
    QWidget::enterEvent(event);

    // Start animation to fade in the hover effect
    m_hoverAnimation->stop();
    m_hoverAnimation->setStartValue(m_hoverOpacity);
    m_hoverAnimation->setEndValue(1.0);
    m_hoverAnimation->start();

    // Let other widgets know this selection was hovered
    emit hovered(m_id);
}

void SelectionWidget::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);

    // Start animation to fade out the hover effect
    m_hoverAnimation->stop();
    m_hoverAnimation->setStartValue(m_hoverOpacity);
    m_hoverAnimation->setEndValue(0.0);
    m_hoverAnimation->start();

    // Reset pressed state when mouse leaves
    m_pressed = false;

    // Let other widgets know the mouse left this selection
    emit left(m_id);
}

void SelectionWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        update();
    }
    QWidget::mousePressEvent(event);
}

void SelectionWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;
        update();

        // Check if release happened within widget bounds
        if (rect().contains(event->pos())) {
            emit clicked(m_id);
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void SelectionWidget::resizeEvent(QResizeEvent *event)
{
    // Handle resize events to ensure proper layout
    QWidget::resizeEvent(event);

    // Force update when size changes
    update();
}
