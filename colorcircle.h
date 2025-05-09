#ifndef COLORCIRCLE_H
#define COLORCIRCLE_H

#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QPropertyAnimation>

class ColorCircle : public QWidget {
    Q_OBJECT

public:
    ColorCircle(const QColor &color, const bool state, QWidget *parent = nullptr);

    void toggleState();

    bool isActive() const { return m_active; }
    void setActive(bool active);

    QColor color() const { return m_color; }
    void setColor(const QColor &color);

    qreal hoverValue() const { return m_hoverValue; }
    void setHoverValue(qreal value);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QColor m_color;
    bool m_active;
    qreal m_hoverValue;
    QPropertyAnimation *m_animation;
};

#endif // COLORCIRCLE_H
