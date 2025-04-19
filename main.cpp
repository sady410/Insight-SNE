#include "menuwindow.h"

#include <QApplication>
// #include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MenuWindow w;
    w.show();
    return a.exec();
}
