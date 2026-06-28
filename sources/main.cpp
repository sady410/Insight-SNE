#include "menuwindow.h"
// #include "thememanager.h"
#include <QApplication>

// void loadStyleSheet(const QString &sheetName)
// {
//     QFile file(":/styles/" + sheetName + ".qss");
//     file.open(QFile::ReadOnly);
//     QString styleSheet = QLatin1String(file.readAll());
//     qApp->setStyleSheet(styleSheet);
// }

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Initialize and apply theme
    // loadStyleSheet("light_theme");

    MenuWindow w;
    // w.setFixedSize(w.size());
    w.show();
    return a.exec();
}
