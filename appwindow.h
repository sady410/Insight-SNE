#ifndef APPWINDOW_H
#define APPWINDOW_H

#include "tsneplot.h"

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
    TsnePlot *tsneplot;
    QScatterSeries *data;
};

#endif // APPWINDOW_H
