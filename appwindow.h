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
    explicit AppWindow(QWidget *parent = nullptr);
    ~AppWindow();

private:
    Ui::AppWindow *ui;
    TsnePlot *tsneplot;
};

#endif // APPWINDOW_H
