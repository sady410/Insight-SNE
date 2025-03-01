#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>

namespace Ui {
class AppWindow;
}

class AppWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AppWindow(QJsonObject json, QWidget *parent = nullptr);
    ~AppWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::AppWindow *ui;
    QJsonObject jsonData; // Deep copy of JSON object
    bool isInitialized;

    // Utility functions for handling JSON conversion
    QList<QList<qreal>> getAxisGradient(const QList<QList<QList<qreal>>> &gradientsList, int axis);
    QList<qreal> jsonArrayToQList(const QJsonArray &jsonArray);
    QList<QList<qreal>> jsonArrayToQList2D(const QJsonArray &jsonArray);
    QList<QList<QList<qreal>>> jsonArrayToQList3D(const QJsonArray &jsonArray);
};

#endif // APPWINDOW_H
