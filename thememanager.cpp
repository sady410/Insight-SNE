#include "thememanager.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>

ThemeManager* ThemeManager::m_instance = nullptr;

ThemeManager* ThemeManager::instance()
{
    if (!m_instance) {
        m_instance = new ThemeManager();
    }
    return m_instance;
}

ThemeManager::ThemeManager(QObject *parent) : QObject(parent), m_currentTheme(Dark)
{
    // Initialize with default colors
    updateColors();
}

ThemeManager::~ThemeManager()
{
}

void ThemeManager::applyTheme(Theme theme)
{
    if (m_currentTheme != theme) {
        m_currentTheme = theme;
        QString themeName;
        switch (theme) {
        case Light: themeName = "Light"; break;
        case Dark: themeName = "Dark"; break;
        case Blue: themeName = "Blue"; break;
        case Custom: themeName = "Custom"; break;
        }
        qDebug() << "Applying theme:" << themeName;
        switch (theme) {
        case Light:
            loadStyleSheet("light_theme");
            break;
        case Dark:
            loadStyleSheet("dark_theme");
            break;
        case Blue:
            loadStyleSheet("blue_theme");
            break;
        case Custom:
            // Custom theme is loaded via setCustomTheme
            break;
        }

        updateColors();
        emit themeChanged(theme);
    }
}

void ThemeManager::setCustomTheme(const QString& stylesheetPath)
{
    QFile file(stylesheetPath);
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        m_currentTheme = Custom;
        updateColors();
        emit themeChanged(Custom);
    } else {
        qWarning() << "Failed to load custom theme:" << stylesheetPath;
    }
}

void ThemeManager::loadStyleSheet(const QString &sheetName)
{
    QFile file(":/styles/" + sheetName + ".qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
    } else {
        qWarning() << "Failed to load stylesheet:" << sheetName;
    }
}

void ThemeManager::updateColors()
{
    // Update the color map based on the current theme
    switch (m_currentTheme) {
    case Light:
        m_colors["primary"] = QColor("#FFFFFF");
        m_colors["secondary"] = QColor("#F0F0F0");
        m_colors["background"] = QColor("#F5F5F5");
        m_colors["text"] = QColor("#333333");
        m_colors["accent"] = QColor("#0078D7");
        break;
    case Dark:
        m_colors["primary"] = QColor("#2D2D30");
        m_colors["secondary"] = QColor("#1E1E1E");
        m_colors["background"] = QColor("#252526");
        m_colors["text"] = QColor("#E6E6E6");
        m_colors["accent"] = QColor("#0E639C");
        break;
    case Blue:
        m_colors["primary"] = QColor("#1C293B");
        m_colors["secondary"] = QColor("#162231");
        m_colors["background"] = QColor("#1F3349");
        m_colors["text"] = QColor("#FFFFFF");
        m_colors["accent"] = QColor("#64B5F6");
        break;
    case Custom:
        // Keep previous colors for custom theme
        break;
    }
}

QColor ThemeManager::primaryColor() const
{
    return m_colors.value("primary", QColor("#2D2D30"));
}

QColor ThemeManager::secondaryColor() const
{
    return m_colors.value("secondary", QColor("#1E1E1E"));
}

QColor ThemeManager::backgroundColor() const
{
    return m_colors.value("background", QColor("#252526"));
}

QColor ThemeManager::textColor() const
{
    return m_colors.value("text", QColor("#E6E6E6"));
}

QColor ThemeManager::accentColor() const
{
    return m_colors.value("accent", QColor("#0E639C"));
}

ThemeManager::Theme ThemeManager::currentTheme() const
{
    return m_currentTheme;
}
