#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QMainWindow>

namespace Ui {
class AppWindow;
}

class AppWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AppWindow(QJsonObject *data, QWidget *parent = nullptr);
    ~AppWindow();

private:
    Ui::AppWindow *ui;

    QList<qreal> jsonArrayToQList(const QJsonArray &jsonArray);
    QList<QList<QList<qreal>>> convertJsonArrayToQList3D(const QJsonArray &jsonArray);
};

#endif // APPWINDOW_H
