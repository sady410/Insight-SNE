#ifndef COLORCIRCLE_H
#define COLORCIRCLE_H

#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QMouseEvent>

class ColorCircle : public QWidget {
    Q_OBJECT

public:
    ColorCircle(const QColor &color, QWidget *parent = nullptr);
    void toggleState();
    bool isActive() const;

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QColor m_color;
    bool m_active;
};

#endif // COLORCIRCLE_H
