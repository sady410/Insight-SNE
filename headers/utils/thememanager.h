#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QColor>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum Theme {
        Light,
        Dark,
        Blue,
        Custom
    };
    Q_ENUM(Theme)

    static ThemeManager* instance();

    void applyTheme(Theme theme);
    void setCustomTheme(const QString& stylesheetPath);

    // Color getters for programmatic use
    QColor primaryColor() const;
    QColor secondaryColor() const;
    QColor backgroundColor() const;
    QColor textColor() const;
    QColor accentColor() const;

    Theme currentTheme() const;

signals:
    void themeChanged(Theme theme);

private:
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();

    static ThemeManager* m_instance;

    Theme m_currentTheme;
    QMap<QString, QColor> m_colors;

    void loadStyleSheet(const QString &sheetName);
    void updateColors();
};

#endif // THEMEMANAGER_H
