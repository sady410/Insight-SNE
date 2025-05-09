#ifndef SELECTIONWIDGET_H
#define SELECTIONWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QEnterEvent>

class SelectionWidget : public QWidget
{
    Q_OBJECT
    // Property for hover animation
    Q_PROPERTY(qreal hoverOpacity READ hoverOpacity WRITE setHoverOpacity)

public:
    explicit SelectionWidget(int id, QWidget *parent = nullptr);

    // Getter and setter for hover opacity property
    qreal hoverOpacity() const { return m_hoverOpacity; }
    void setHoverOpacity(qreal opacity);

protected:
    // Event handlers
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

signals:
    void hovered(int id);
    void left(int id);
    void clicked(int id);

private:
    int m_id;
    qreal m_hoverOpacity;
    bool m_pressed = false;
    QPropertyAnimation *m_hoverAnimation;
};

#endif // SELECTIONWIDGET_H
