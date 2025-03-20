#include "menuwindow.h"

#include <QApplication>
// #include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // // Set the application style to "Fusion"
    // QApplication::setStyle(QStyleFactory::create("Fusion"));

    // // Create a light palette
    // QPalette lightPalette;

    // // Set light colors for the palette
    // lightPalette.setColor(QPalette::Window, QColor(240, 240, 240)); // Light gray background
    // lightPalette.setColor(QPalette::WindowText, Qt::black);        // Black text
    // lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));   // White background for input widgets
    // lightPalette.setColor(QPalette::AlternateBase, QColor(240, 240, 240)); // Light gray for alternate backgrounds
    // lightPalette.setColor(QPalette::ToolTipBase, Qt::white);       // White tooltip background
    // lightPalette.setColor(QPalette::ToolTipText, Qt::black);       // Black tooltip text
    // lightPalette.setColor(QPalette::Text, Qt::black);              // Black text for input widgets
    // lightPalette.setColor(QPalette::Button, QColor(240, 240, 240)); // Light gray for buttons
    // lightPalette.setColor(QPalette::ButtonText, Qt::black);        // Black text for buttons
    // lightPalette.setColor(QPalette::BrightText, Qt::red);          // Bright text (e.g., for errors)
    // lightPalette.setColor(QPalette::Highlight, QColor(0, 120, 215)); // Blue highlight
    // lightPalette.setColor(QPalette::HighlightedText, Qt::white);   // White text for highlighted items

    // Apply the light palette to the application
    // QApplication::setPalette(lightPalette);

    MenuWindow w;
    w.show();
    return a.exec();
}
