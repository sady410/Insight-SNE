#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

#include <QObject>
#include <QColor>
#include <QList>
#include <QPoint>
#include <QHash>
#include <QStack>
#include <QSet>

class SelectionManager : public QObject {
    Q_OBJECT

public:
    struct Selection {
        int id;
        bool state;
        QColor color;
        QString name;
        QList<int> indices;

        Selection(int id = -1, const QColor& color = Qt::black, const QString name = "", const QList<int>& indices = {}, const bool state = true)
            : id(id), state(state), color(color), name(name), indices(indices) {}
    };

    explicit SelectionManager(QObject* parent = nullptr);

public slots:
    void createSelection(const QList<int>& indices, const bool enableDelete, const bool state, const QString name = "");
    void createSelection(const QList<int>& indices);
    void removeSelection(int id);

    void findHoveredSelectionAndEmitSignal(int id);

    void changeSelectionStateAndEmitSignal(int id);

    void selectionClickedInBarPlot(const QString &label, int id);

    void selectionFreatureArrowCheckboxChanged(int id, const QString &featureName, const bool checked);

signals:
    void selectionCreated(int id, const QColor &color, const QList<int>& indices, const bool enableDelete, const bool state, const QString name);
    void selectionRemoved(int id);

    // Signals used when hovering in MenuForm
    void startHighlightSelection(const QColor &color, const QList<int>& indices);
    void stopHighlightSelection();

    // Signals used when changing a selection's state
    void selectionEnabled(int id, const QColor &color, const QList<int>& indices, const bool enableDelete, const bool state);
    void selectionDisabled(int id);

    // Add or remove arrows to tsne plot
    void addArrows(int selectionId, const QList<int>& indices, const QString &featureName);
    void removeArrows(int selectionId, const QString &featureName);

private:
    QList<QColor> m_colorList;
    QSet<QColor> m_usedColors;

    QHash<int, Selection> m_selections; // Use QHash for efficient lookups by ID
    QStack<int> m_availableIds; // Stack to store reusable IDs
    int m_nextId; // Counter for generating new IDs

    int generateUniqueId();
    QColor getAvailableColor();
    void releaseColor(const QColor& color);
};

#endif // SELECTIONMANAGER_H
